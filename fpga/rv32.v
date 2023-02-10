/*
RISC-V CPU RV32ia_zicsr
- 32 bit version
- integer
- atomic instructions
- control and status register
- privileged modes (machine,supervisor, user)
- virtual memory mapping
*/
`default_nettype none

module rv32(
	input     		i_rst, 
	input	 		i_clk,
	output [31:0]	o_addr,
	output [31:0]	o_dat_w,
	output 	[3:0]   o_we, 
	input  [31:0]   i_dat_r,
	output	 		o_stb,
	output [1:0]	o_led,
	input      		i_ack,
	input      		i_mtip,
	input      		i_msip,
	input      		i_meip,
	input			i_seip
);

// privileged mode is ouput to LED
// 00 - U user mode
// 01 - S supervisor mode
// 11 - M machine mode
assign o_led=privileged_mode;

// start boot loader at 0x00000000
parameter RESET_ADDR       = 32'h00000000;

//#### state of CPU #######################################
reg r_stb;					//strobe signal to sba bus
always @(posedge i_clk)
	if (i_rst | r_decode | (r_load | r_store) & virtual_memory_ack) r_stb <= 1;
	else r_stb <= 0;

wire fetch=(r_decode & ~(instr_Load|instr_Store|instr_Amoswap)) | ((r_load&~instr_Amoswap|r_store)&virtual_memory_ack); 
reg r_fetch;					//fetch instruction
always @(posedge i_clk)
	if (i_rst | fetch) r_fetch <= 1;
	else if (r_fetch & virtual_memory_ack) r_fetch <= 0;

wire decode = (r_fetch & virtual_memory_ack); 
reg r_decode;					//decode instruction
always @(posedge i_clk)
	if (i_rst | r_decode) r_decode <= 0;
	else if (decode) r_decode <= 1;

reg r_load;					//load instruction
always @(posedge i_clk)
	if (i_rst | (r_load & virtual_memory_ack)) r_load <= 0;
	else if (r_decode & (instr_Load|instr_Amoswap)) r_load <= 1;

reg r_store;					//store instruction
always @(posedge i_clk)
	if (i_rst | (r_store & virtual_memory_ack)) r_store <= 0;
	else if ((r_decode & instr_Store) | (r_load & instr_Amoswap & virtual_memory_ack)) r_store <= 1;

//#### Register ###########################################
reg  [31:0] r_pc;				// The program counter.
always @(posedge i_clk)
	if (i_rst) r_pc <= RESET_ADDR;
	else if (interrupt_m) r_pc <= csr_mtvec;
	else if (interrupt_s) r_pc <= csr_stvec;
	else if (fetch) r_pc <= PC_new;

//WTF
reg [31:0] r_instruction;			// instruction
always @(posedge i_clk)
	if (i_rst) r_instruction <= 3;
	else if (decode) r_instruction <= i_dat_r;

reg [31:0] r_reg[31:0];
initial r_reg[0] = 32'd0;	//register x[0] hardwire to zero
reg [31:0] r_rs1;						// register rs1
always @(posedge i_clk)
	if (decode) r_rs1 <= r_reg[i_dat_r[19:15]];

reg [31:0] r_rs2;						// register rs2
always @(posedge i_clk)
	if (decode) r_rs2 <= r_reg[i_dat_r[24:20]];
reg [4:0] r_rs2_r;						// register rs2
always @(posedge i_clk)
	if (decode) r_rs2_r <= i_dat_r[24:20];
wire [4:0] instr_rd = r_instruction[11:7];		// destination register


//#### instruction decoder ################################
wire [7:0] instr_funct3 = 8'b00000001 << r_instruction[14:12];

wire [31:0] instr_Uimm={    r_instruction[31],   r_instruction[30:12], {12{1'b0}}};
wire [31:0] instr_Iimm={{21{r_instruction[31]}}, r_instruction[30:20]};
wire [31:0] instr_Simm={{21{r_instruction[31]}}, r_instruction[30:25],r_instruction[11:7]};
wire [31:0] instr_Bimm={{20{r_instruction[31]}}, r_instruction[7],r_instruction[30:25],r_instruction[11:8],1'b0};
wire [31:0] instr_Jimm={{12{r_instruction[31]}}, r_instruction[19:12],r_instruction[20],r_instruction[30:21],1'b0};
 
wire [6:0] instr_opcode = r_instruction[6:0];
wire instr_Load    =  (instr_opcode[6:2] == 5'b00000); // rd <- mem[rs1+Iimm]
wire instr_ALUimm  =  (instr_opcode[6:2] == 5'b00100); // rd <- rs1 OP Iimm
wire instr_AUIPC   =  (instr_opcode[6:2] == 5'b00101); // rd <- PC + Uimm
wire instr_Store   =  (instr_opcode[6:2] == 5'b01000); // mem[rs1+Simm] <- rs2
wire instr_ALUreg  =  (instr_opcode[6:2] == 5'b01100); // rd <- rs1 OP rs2
wire instr_LUI     =  (instr_opcode[6:2] == 5'b01101); // rd <- Uimm
wire instr_Branch  =  (instr_opcode[6:2] == 5'b11000); // if(rs1 OP rs2) PC<-PC+Bimm
wire instr_JALR    =  (instr_opcode[6:2] == 5'b11001); // rd <- PC+4; PC<-rs1+Iimm
wire instr_JAL     =  (instr_opcode[6:2] == 5'b11011); // rd <- PC+4; PC<-PC+Jimm
wire instr_SYSTEM  =  (instr_opcode[6:2] == 5'b11100); // rd <- CSR <- rs1/uimm5
wire instr_FENCE   =  (instr_opcode[6:2] == 5'b00011); // rd <- CSR <- rs1/uimm5
wire instr_Amoswap =  (instr_opcode[6:2] == 5'b01011); // rd <- CSR <- rs1/uimm5


//#### ALU - arithemtic and logic unit ####################
wire instr_ALU = instr_ALUimm | instr_ALUreg;
wire [31:0] alu_In1 = r_rs1;
wire [31:0] alu_In2 = instr_ALUreg | instr_Branch ? r_rs2 : instr_Iimm;

wire [31:0] alu_Plus = alu_In1 + alu_In2;
wire [32:0] alu_Minus = {1'b1, ~alu_In2} + {1'b0,alu_In1} + 33'b1;
wire alu_lt  = (alu_In1[31]^alu_In2[31])? alu_In1[31] : alu_Minus[32];
wire alu_ltu = alu_Minus[32];
wire alu_eq  = (alu_Minus[31:0] == 0);

wire [31:0] alu_leftShift0 = alu_In1;
wire [31:0] alu_rightShift0 = alu_In1;
wire alu_rightShiftIn = r_instruction[30] & alu_rightShift0[31];
wire [31:0] alu_rightShift1 = alu_In2[4]? {{16{alu_rightShiftIn}},alu_rightShift0[31:16]} : alu_rightShift0;
wire [31:0] alu_rightShift2 = alu_In2[3]? {{ 8{alu_rightShiftIn}},alu_rightShift1[31: 8]} : alu_rightShift1;
wire [31:0] alu_rightShift3 = alu_In2[2]? {{ 4{alu_rightShiftIn}},alu_rightShift2[31: 4]} : alu_rightShift2;
wire [31:0] alu_rightShift4 = alu_In2[1]? {{ 2{alu_rightShiftIn}},alu_rightShift3[31: 2]} : alu_rightShift3;
wire [31:0] alu_rightShift5 = alu_In2[0]? {{ 1{alu_rightShiftIn}},alu_rightShift4[31: 1]} : alu_rightShift4;

wire [31:0] alu_leftShift1 = alu_In2[4]? (alu_leftShift0 << 16) : alu_leftShift0;
wire [31:0] alu_leftShift2 = alu_In2[3]? (alu_leftShift1 <<  8) : alu_leftShift1;
wire [31:0] alu_leftShift3 = alu_In2[2]? (alu_leftShift2 <<  4) : alu_leftShift2;
wire [31:0] alu_leftShift4 = alu_In2[1]? (alu_leftShift3 <<  2) : alu_leftShift3;
wire [31:0] alu_leftShift5 = alu_In2[0]? (alu_leftShift4 <<  1) : alu_leftShift4;

wire [31:0] alu_Out =
     (instr_funct3[0]  ? r_instruction[30] & r_instruction[5] ? alu_Minus[31:0] : alu_Plus : 32'b0) |
     (instr_funct3[1]  ? alu_leftShift5                                                    : 32'b0) |
     (instr_funct3[2]  ? {31'b0, alu_lt}                                                   : 32'b0) |
     (instr_funct3[3]  ? {31'b0, alu_ltu}                                                  : 32'b0) |
     (instr_funct3[4]  ? alu_In1 ^ alu_In2                                                 : 32'b0) |
     (instr_funct3[5]  ? alu_rightShift5                                                   : 32'b0) |
     (instr_funct3[6]  ? alu_In1 | alu_In2                                                 : 32'b0) |
     (instr_funct3[7]  ? alu_In1 & alu_In2                                                 : 32'b0) ;

//#### predicate ##########################################
wire alu_predicate =
        instr_funct3[0] &  alu_eq  | // BEQ
        instr_funct3[1] & !alu_eq  | // BNE
        instr_funct3[4] &  alu_lt  | // BLT
        instr_funct3[5] & !alu_lt  | // BGE
        instr_funct3[6] &  alu_ltu | // BLTU
        instr_funct3[7] & !alu_ltu ; // BGEU
  

//#### LOAD STORE #########################################
wire [31:0] load_store_addr = instr_Amoswap ? r_rs1 : r_rs1[31:0] + (r_instruction[5] ? instr_Simm[31:0] : instr_Iimm[31:0]);
//load
wire load_store_byteAccess     = r_instruction[13:12] == 2'b00;
wire load_store_halfwordAccess = r_instruction[13:12] == 2'b01;

wire load_sign = !r_instruction[14] & (load_store_byteAccess ? load_byte[7] : load_halfword[15]);

wire [31:0] load_data     = load_store_byteAccess ? {{24{load_sign}},     load_byte} :
                            load_store_halfwordAccess ? {{16{load_sign}}, load_halfword} :  i_dat_r;
wire [15:0] load_halfword = load_store_addr[1] ? i_dat_r[31:16] : i_dat_r[15:0];
wire  [7:0] load_byte     = load_store_addr[0] ? load_halfword[15:8] : load_halfword[7:0];
//store
assign o_dat_w[ 7: 0] = r_rs2[7:0];
assign o_dat_w[15: 8] = load_store_addr[0] ? r_rs2[7:0]  : r_rs2[15: 8];
assign o_dat_w[23:16] = load_store_addr[1] ? r_rs2[7:0]  : r_rs2[23:16];
assign o_dat_w[31:24] = load_store_addr[0] ? r_rs2[7:0]  :
                        load_store_addr[1] ? r_rs2[15:8] : r_rs2[31:24];

wire [3:0] store_wmask = load_store_byteAccess ?
								(load_store_addr[1] ?
									(load_store_addr[0] ? 4'b1000 : 4'b0100) :
									(load_store_addr[0] ? 4'b0010 : 4'b0001)
							) :
								load_store_halfwordAccess ?
									(load_store_addr[1] ? 4'b1100 : 4'b0011) :
									4'b1111;

wire[3:0] virtual_memory_we = {4{r_store}} & store_wmask;
//#### programm counter ###################################
wire [31:0] pc_plusImm = r_pc + ( r_instruction[3] ? instr_Jimm[31:0] : r_instruction[4] ? instr_Uimm[31:0] : instr_Bimm[31:0] );
wire [31:0] pc_plus4 = r_pc + 4;

//#### write back #########################################
wire writeBack = (r_decode & ~(instr_Branch | instr_Store | instr_Load | instr_Amoswap)) | (r_load & virtual_memory_ack);
wire [31:0] writeBackData  =
      (instr_SYSTEM                ? CSR_read    : 32'b0) |  // SYSTEM
      (instr_LUI                   ? instr_Uimm  : 32'b0) |  // LUI
      (instr_ALU                   ? alu_Out     : 32'b0) |  // ALUreg, ALUimm
      (instr_AUIPC                 ? pc_plusImm  : 32'b0) |  // AUIPC
      (instr_JALR | instr_JAL      ? pc_plus4    : 32'b0) |  // JAL, JALR
      (instr_Load | instr_Amoswap  ? load_data   : 32'b0) ;  // Load

reg [31:0] r_amo;
always @(posedge i_clk)
	if (i_rst) r_amo <= 0;
	else if (writeBack) r_amo <= writeBackData;

always @(posedge i_clk)
	if (writeBack & (instr_rd != 5'd0)) r_reg[instr_rd] <= writeBackData;
	else if ((r_rs2_r!=5'd0) & instr_Amoswap & r_store & r_stb) r_reg[r_rs2_r] <= r_amo; 

//#### jump - JAL, JALR ###################################
wire instr_Jump      =   instr_JAL | (instr_Branch & alu_predicate);
wire [31:0] PC_new   =   instr_ECALL ? csr_stvec :
						 instr_JALR ? {alu_Plus[31:1],1'b0} :
                         instr_Jump ? pc_plusImm :
                         instr_MRET ? csr_mepc :
                         instr_SRET ? csr_sepc :
                         pc_plus4;

//#### CSR - control and status register #################
wire [11:0] instr_csr = r_instruction[31:20];
wire csr_write = instr_CSRW & ~exception_csr & r_decode;
wire [31:0] CSR_modifier = r_instruction[14] ? {27'd0, r_instruction[19:15]} : r_rs1; 
wire [31:0] CSR_write = (r_instruction[13:12] == 2'b10) ? CSR_modifier | CSR_read  :
                        (r_instruction[13:12] == 2'b11) ? ~CSR_modifier & CSR_read :
                     /* (r_instruction[13:12] == 2'b01) ? */  CSR_modifier ;

//#### Interrupt logic ####################################
wire exception_page_fault;
wire exception_csr = instr_CSRW & |(instr_csr[9:8] & ~privileged_mode);
wire exception_illegal_instruction = 0;//exception_csr | (~&instr_opcode[1:0]) | ~(instr_FENCE|instr_Load|instr_ALUimm|instr_AUIPC|instr_Store|instr_ALUreg|instr_LUI|instr_Branch|instr_JALR|instr_JAL|instr_SYSTEM|instr_Amoswap|instr_CSRW);
wire [31:0] csr_mep = {16'd0,exception_page_fault&r_store,1'b0,exception_page_fault&r_load,exception_page_fault&r_fetch,r_decode&instr_ECALL&privileged_mode[1],1'b0,r_decode&instr_ECALL&~privileged_mode[1]&privileged_mode[0],r_decode&instr_ECALL&~privileged_mode[0],5'd0,exception_illegal_instruction&r_decode,2'b0};

wire exception_m = 0;//|(csr_mep & ~csr_medeleg);
wire exception_s = 0;//|(csr_mep & csr_medeleg) & ~privileged_mode[1];


wire interrupt_m = fetch & (|(csr_mip & csr_mie & ~csr_mideleg)) & (~privileged_mode[1] | (privileged_mode[1]&r_mie));        
wire interrupt_s = fetch & (|(csr_sip & csr_sie )) & (~privileged_mode[0] | (~privileged_mode[1]&privileged_mode[0]&r_sie));

//#### mret instruction ###################################
wire instr_ECALL = instr_SYSTEM & instr_funct3[0] & (r_instruction[31:20]==12'h000) & r_decode;
wire instr_MRET = instr_SYSTEM & instr_funct3[0] & (r_instruction[31:20]==12'h302) & r_decode;
wire instr_SRET = instr_SYSTEM & instr_funct3[0] & (r_instruction[31:20]==12'h102) & r_decode;
wire instr_SFENCE_VMA = instr_SYSTEM & instr_funct3[0] & (r_instruction[31:25]==7'h09) & r_decode;
wire instr_CSRW = instr_SYSTEM & (r_instruction[14:12] != 0) & r_decode;

//#### privileged modes status
reg [1:0] privileged_mode;					//machine 11, supervisor 01, user 00
always @(posedge i_clk)
	if (i_rst) privileged_mode <=2'b11;
	else if (exception_m) privileged_mode <= 2'b11;
	else if (exception_s) privileged_mode <= 2'b01;
	else if (interrupt_m) privileged_mode <= 2'b11;
	else if (interrupt_s) privileged_mode <= 2'b01;
	else if (instr_ECALL) privileged_mode <= 2'b01;
	else if (instr_MRET) privileged_mode <= r_mpp;
	else if (instr_SRET & ~privileged_mode[1]) privileged_mode <= {1'b0,r_spp};
reg r_sie;
always @(posedge i_clk)
	if (i_rst) r_sie <= 0;
	else if (exception_m) r_sie <= r_sie;
	else if (exception_s) r_sie <= 0;
	else if (interrupt_m) r_sie <= r_sie;
	else if (interrupt_s) r_sie <= 0;
	else if (csr_write & sel_mstatus) r_sie <= CSR_write[1];
	else if (csr_write & sel_sstatus) r_sie <= CSR_write[1];
reg r_mie;
always @(posedge i_clk)
	if (i_rst) r_mie <= 0;
	else if (exception_m) r_mie <= 0;
	else if (exception_s) r_mie <= r_mie;
	else if (interrupt_m) r_mie <= 0;
	else if (interrupt_s) r_mie <= r_mie;
	//else if (instr_MRET) r_mie <= r_mpie;
	else if (csr_write & sel_mstatus) r_mie <= CSR_write[3];
reg r_spie;
always @(posedge i_clk)
	if (i_rst) r_spie <= 0;
	else if (exception_m) r_spie <= r_spie;
	else if (exception_s) r_spie <= r_sie;
	else if (interrupt_m) r_spie <= r_spie;
	else if (interrupt_s) r_spie <= r_sie;

reg r_mpie;
always @(posedge i_clk)
	if (i_rst) r_mpie <= 0;
	else if (exception_m) r_mpie <= r_mie;
	else if (exception_s) r_mpie <= r_mpie;
	else if (interrupt_m) r_mpie <= r_mie;
	else if (interrupt_s) r_mpie <= r_mpie;
	else if (csr_write & sel_mstatus) r_mpie <= CSR_write[7];
reg r_spp;
always @(posedge i_clk)
	if (i_rst) r_spp <= 0;
	else if (exception_m) r_spp <= r_spp;
	else if (exception_s) r_spp <= privileged_mode[0];
	else if (interrupt_m) r_spp <= r_spp;
	else if (interrupt_s) r_spp <= privileged_mode[0];
	else if (csr_write & sel_mstatus) r_spp <= CSR_write[8];
	else if (csr_write & sel_sstatus) r_spp <= CSR_write[8];
reg [1:0] r_mpp;
always @(posedge i_clk)
	if (i_rst) r_mpp <= 0;
	else if (exception_m) r_mpp <= privileged_mode;
	else if (exception_s) r_mpp <= r_mpp;
	else if (interrupt_m) r_mpp <= privileged_mode;
	else if (interrupt_s) r_mpp <= r_mpp;
	else if (instr_MRET) r_mpp <= 0;
	else if (csr_write & sel_mstatus) r_mpp <= CSR_write[12:11];
wire sel_mstatus = (instr_csr == 12'h300);
wire [31:0] csr_mstatus={19'd0,r_mpp,2'b00,r_spp,r_mpie,1'b0,r_spie,1'b0,r_mie,1'b0,r_sie,1'b0};
wire [31:0] csr_sstatus={19'd0,2'b00,2'b00,r_spp,1'b0,1'b0,r_spie,1'b0,1'b0,1'b0,r_sie,1'b0};
wire sel_sstatus = (instr_csr == 12'h100);

//delegation of exception and interrupt
reg  [31:0] csr_medeleg;	//machine mode exception delegate
wire sel_medeleg = (instr_csr == 12'h302);
always @(posedge i_clk)
	if (i_rst) csr_medeleg <= 0;
	else if (csr_write & sel_medeleg) csr_medeleg <= CSR_write[31:0];
reg  [31:0] csr_mideleg;	//machine mode interrupt delegate
wire sel_mideleg = (instr_csr == 12'h303);
always @(posedge i_clk)
	if (i_rst) csr_mideleg <= 0;
	else if (csr_write & sel_mideleg) csr_mideleg <= CSR_write[31:0] & 32'b00000000000000000000_0010_0010_0010;

//interrupt pending
reg [31:0] csr_mie;
wire sel_mie = (instr_csr == 12'h304);
always @(posedge i_clk)
	if (i_rst) csr_mie <= 0;
	else if (csr_write & sel_mie) csr_mie <= CSR_write;
	else if (csr_write & sel_sie) csr_mie <= (csr_mie   & ~csr_mideleg) | (CSR_write & csr_mideleg);

wire  [31:0] csr_sie = csr_mie & csr_mideleg;	//supervisor interrupt enable
wire sel_sie     = (instr_csr == 12'h104);

//interrupt pending
reg r_ssip;
reg r_stip;
reg r_seip;
wire sel_mip = (instr_csr == 12'h344);
wire [31:0] csr_mip = {20'd0,i_meip,1'b0,r_seip|i_seip,1'b0,i_mtip,1'b0,r_stip,1'b0,i_msip,1'b0,r_ssip,1'b0};
always @(posedge i_clk)
	if (i_rst) r_ssip <= 0;
	else if (csr_write & sel_mip) r_ssip <= CSR_write[1];
	else if (csr_write & sel_sip & csr_mideleg[1]) r_ssip <= CSR_write[1];
always @(posedge i_clk)
	if (i_rst) r_stip <= 0;
	else if (csr_write & sel_mip) r_stip <= CSR_write[5];
always @(posedge i_clk)
	if (i_rst) r_seip <= 0;
	else if (csr_write & sel_mip) r_seip <= CSR_write[9];
wire  [31:0] csr_sip = csr_mip & csr_mideleg;
wire sel_sip     = (instr_csr == 12'h144);

//trap vector
reg  [31:0] csr_mtvec;   // The address of the interrupt handler.
wire sel_mtvec   = (instr_csr == 12'h305);
always @(posedge i_clk)
	if (i_rst) csr_mtvec <= 0;
	else if (csr_write & sel_mtvec) csr_mtvec <= CSR_write[31:0];
reg  [31:0] csr_stvec;	//supervisor trap vector
wire sel_stvec   = (instr_csr == 12'h105);
always @(posedge i_clk)
	if (i_rst) csr_stvec <= 0;
	else if (csr_write & sel_stvec) csr_stvec <= CSR_write[31:0];

//scratch
reg  [31:0] csr_mscratch;	//machine mode scratch
wire sel_mscratch= (instr_csr == 12'h340);
always @(posedge i_clk)
	if (i_rst) csr_mscratch <= 0;
	else if (csr_write & sel_mscratch) csr_mscratch <= CSR_write[31:0];
reg  [31:0] csr_sscratch;	//supervisor scratch
wire sel_sscratch= (instr_csr == 12'h140);
always @(posedge i_clk)
	if (i_rst) csr_sscratch <= 0;
	else if (csr_write & sel_sscratch) csr_sscratch <= CSR_write[31:0];

//exception programm counter
reg  [31:0] csr_mepc;    // machine mode exception program counter.
wire sel_mepc    = (instr_csr == 12'h341);
always @(posedge i_clk)
	if (i_rst) csr_mepc <= 0;
	else if (exception_m) csr_mepc <= PC_new;//Achtung bei normalen exceptions  r_pc
	else if (exception_s) csr_mepc <= csr_mepc;
	else if (interrupt_m) csr_mepc <= PC_new;
	else if (interrupt_s) csr_mepc <= csr_mepc;
	else if (csr_write & sel_mepc) csr_mepc <= CSR_write[31:0];

reg  [31:0] csr_sepc;	//supervisor exception programm counter
wire sel_sepc    = (instr_csr == 12'h141);
always @(posedge i_clk)
	if (i_rst) csr_sepc <= 0;
	else if (exception_m) csr_sepc <= csr_sepc;
	else if (exception_s) csr_sepc <= r_pc;
	else if (interrupt_m) csr_sepc <= csr_sepc;
	else if (interrupt_s) csr_sepc <= PC_new;
	else if (instr_ECALL) csr_sepc <= r_pc;
	else if (csr_write & sel_sepc) csr_sepc <= CSR_write[31:0];

//cause of exception/interrupt
reg  [31:0] csr_mcause;  // machine mode cause
wire sel_mcause  = (instr_csr == 12'h342);
always @(posedge i_clk)
	if (i_rst) csr_mcause <= 0;
	else if (exception_m) csr_mcause <= 32'd42;//Achtung bei normalen exceptions  r_pc
	else if (exception_s) csr_mcause <= csr_mcause;
	else if (interrupt_m) csr_mcause <= csr_mip[7]? {1'b1,31'd7}: csr_mip[11]? {1'b1,31'd11}:csr_mip[9]?{1'b1,31'd9}:32'd42;
	else if (interrupt_s) csr_mcause <= csr_mcause;

reg  [31:0] csr_scause;	//supervisor exception cause
wire sel_scause  = (instr_csr == 12'h142);
always @(posedge i_clk)
	if (i_rst) csr_scause <= 0;
	else if (exception_m) csr_scause <= csr_scause;//Achtung bei normalen exceptions  r_pc
	else if (exception_s) csr_scause <= 32'd42;
	else if (interrupt_m) csr_scause <= csr_scause;
	else if (interrupt_s) csr_scause <= csr_mip[1]? {1'b1,31'd1}: csr_mip[9]?{1'b1,31'd9}:32'd41;
	else if (instr_ECALL) csr_scause <= 32'd8;

//trap value
reg  [31:0] csr_mtval;   //machine mode trap value
wire sel_mtval   = (instr_csr == 12'h343);
always @(posedge i_clk)
	if (i_rst) csr_mtval <= 0;
	else if (exception_m) csr_mtval <= exception_illegal_instruction?r_instruction: exception_page_fault?virtual_memory_addr:32'd42;
	else if (exception_s) csr_mtval <= csr_mtval;
	else if (csr_write & sel_mtval) csr_mtval <= CSR_write[31:0];
reg  [31:0] csr_stval;	//supervisor trap value
wire sel_stval   = (instr_csr == 12'h143);
always @(posedge i_clk)
	if (i_rst) csr_stval <= 0;
	else if (exception_m) csr_stval <= csr_stval;
	else if (exception_s) csr_stval <= exception_illegal_instruction?r_instruction: exception_page_fault?virtual_memory_addr:32'd42;
	else if (csr_write & sel_stval) csr_stval <= CSR_write[31:0];


reg  [31:0] csr_satp;	//supervispr address translation page
wire sel_satp    = (instr_csr == 12'h180);
always @(posedge i_clk)
	if (i_rst) csr_satp <= 0;
	else if (csr_write & sel_satp) csr_satp <= CSR_write[31:0];








reg  [63:0] csr_cycles;  // Cycle counter
wire sel_cycles  = (instr_csr == 12'hC00);
wire sel_cyclesh = (instr_csr == 12'hC80);
always @(posedge i_clk)
	if(i_rst) csr_cycles <= 0;
	else csr_cycles <= csr_cycles + 1;

//#### Read CSRs ##########################################
wire [31:0] CSR_read =
     (sel_sstatus ? csr_sstatus         : 32'b0) |
     (sel_sie     ? csr_sie             : 32'b0) |
     (sel_stvec   ? csr_stvec           : 32'b0) |
     (sel_sscratch? csr_sscratch        : 32'b0) |
     (sel_sepc    ? csr_sepc            : 32'b0) |
     (sel_scause  ? csr_scause          : 32'b0) |
     (sel_stval   ? csr_stval           : 32'b0) |
     (sel_sip     ? csr_sip             : 32'b0) |
     (sel_satp    ? csr_satp            : 32'b0) |
     (sel_mstatus ? csr_mstatus         : 32'b0) |
     (sel_medeleg ? csr_medeleg         : 32'b0) |
     (sel_mideleg ? csr_mideleg         : 32'b0) |
     (sel_mie     ? csr_mie             : 32'b0) |
     (sel_mtvec   ? csr_mtvec           : 32'b0) |
     (sel_mscratch? csr_mscratch        : 32'b0) |
     (sel_mepc    ? csr_mepc            : 32'b0) |
     (sel_mcause  ? csr_mcause          : 32'b0) |
     (sel_mtval   ? csr_mtval           : 32'b0) |
     (sel_mip     ? csr_mip             : 32'b0) |
     (sel_cycles  ? csr_cycles[31:0]    : 32'b0) |
     (sel_cyclesh ? csr_cycles[63:32]   : 32'b0) ;


//#### virtual memory paging ##############################
wire [31:0] virtual_memory_addr = (r_fetch) ? r_pc: (r_load | r_store)? load_store_addr:r_pc;
wire virtual_memory_ack;

vma VMA(
	.i_rst(i_rst),
	.i_clk(i_clk),
	.i_v_addr(virtual_memory_addr),
	.i_v_stb(r_stb),
	.i_v_we(virtual_memory_we),
	.o_v_ack(virtual_memory_ack),
	.o_p_addr(o_addr),
	.o_p_stb(o_stb),
	.o_p_we(o_we),
	.i_p_ack(i_ack),
	.i_p_dat_r(i_dat_r),
	.i_satp(csr_satp),
	.i_smode(~privileged_mode[1]),
	.i_sfence_vma(instr_SFENCE_VMA),
	.o_exception(exception_page_fault)
);


endmodule
