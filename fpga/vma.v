`default_nettype none
module vma(
	input		i_clk,
	input		i_rst,
	input	[31:0]	i_v_addr,
	input		i_v_stb,
	input	[3:0]	i_v_we,
	output		o_v_ack,
	output	[31:0]	o_p_addr,
	output		o_p_stb,
	output	[3:0]	o_p_we,
	input		i_p_ack,
	input	[31:0]	i_p_dat_r,
	input	[31:0]	i_satp,
	input		i_smode,
	input		i_sfence_vma,
	output		o_exception
);
//satp supervisor address translation and protection register
wire [21:0] 	satp_ppn  = i_satp[21:0];
wire 		satp_mode = i_satp[31] & i_smode;
wire rst = i_rst|i_sfence_vma|exception;
wire o_exception=0;
//translation process
assign o_p_addr[11:0]  = ~satp_mode? i_v_addr[11:0]: walk1? {i_v_addr[31:22],2'd0} : walk2? {i_v_addr[21:12],2'd0} : i_v_addr[11:0];
assign o_p_addr[31:12] = ~satp_mode? i_v_addr[31:12] : walk1? satp_ppn[19:0] : pte[29:10];
wire exception = (walk1|walk2) & i_p_ack & ~i_p_dat_r[0];
assign o_p_stb = ~satp_mode? i_v_stb: (walk3_stb|walk1_stb|walk2_stb);
assign o_v_ack = ~satp_mode? i_p_ack: (walk3 & i_p_ack);
assign o_p_we  = (~satp_mode | walk3)? i_v_we: 4'd0;

wire start_walk = satp_mode & i_v_stb;

reg walk1;
always @(posedge i_clk)
	if (rst) walk1 <= 0;
	else if (start_walk) walk1 <= 1;
	else if (walk1 & i_p_ack) walk1 <= 0;
reg walk2;
always @(posedge i_clk)
	if (rst) walk2 <= 0;
	else if (walk1 & i_p_ack) walk2 <= 1;
	else if (walk2 & i_p_ack) walk2 <= 0;
reg walk3;
always @(posedge i_clk)
	if (rst) walk3 <= 0;
	else if (walk2 & i_p_ack) walk3 <= 1;
	else if (walk3 & i_p_ack) walk3 <= 0;
reg walk1_stb;
always @(posedge i_clk)
	if (rst | walk1_stb) walk1_stb <= 0;
	else if (start_walk) walk1_stb <= 1;
reg walk2_stb;
always @(posedge i_clk)
	if (rst | walk2_stb) walk2_stb <= 0;
	else if (walk1 & i_p_ack) walk2_stb <= 1;
reg walk3_stb;
always @(posedge i_clk)
	if (rst | walk3_stb) walk3_stb <= 0;
	else if (walk2 & i_p_ack) walk3_stb <= 1;

reg [31:0] pte;
always @(posedge i_clk)
	if (rst) pte <= 0;
	else if ((walk1|walk2) & i_p_ack) pte <= i_p_dat_r;

endmodule
