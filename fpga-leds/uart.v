// 1 startbit
// 8 databits
// 2 stopbit

`default_nettype none
module uart(
	input wire i_rst,
	input wire i_clk,
	input wire [2:0] i_addr,
	input wire i_stb,
	input wire [3:0] i_we,
	output wire o_ack,
	input wire [31:0] i_dat_w,
	output wire [31:0] o_dat_r,
	output wire o_tx,
	input wire i_rx,
	output wire o_int
);
	
	assign o_ack = i_stb;
	// The Transmitter
	wire tx_baud = (TBAUD==0);	
	wire thr_load = i_stb & i_we[0] & addr_is0;
	wire tsr_load = ~THE & (TSE | tsr_unload);
	wire tsr_unload = ~TSE & (TSR == 3) & tx_baud;
	
	//THR Transmitter holding register
	reg[7:0] THR;
	always @(posedge i_clk)
		if (i_rst) THR <=0;
		else if (thr_load) THR <= i_dat_w[7:0];

	//THE Transmitter holding register is empty
	reg THE;
	always @(posedge i_clk)
		if (i_rst) THE <= 1;
		else if (thr_load) THE <= 0;
		else if (tsr_load) THE <= 1;
	reg THE_old;
	always @(posedge i_clk)
		if (i_rst) THE_old <= 1;
		else THE_old <= THE;
	
	//TSR Transmitter shifting register	
	reg [11:0] TSR;
	always @(posedge i_clk)
		if (i_rst) TSR <= 12'b111111111111;
		else if (tsr_load) TSR <= {3'b111,THR,1'b0};
		else if (tx_baud & ~TSE) TSR <= {1'b0,TSR[11:1]};
		else if (TSE) TSR <= 12'b111111111111;

	//TSE Transmitter shifting register is empty
	reg TSE;
	always @(posedge i_clk)
		if (i_rst) TSE <= 1;
		else if (tsr_load) TSE <= 0;
		else if (tsr_unload) TSE <= 1;
	
	assign o_tx = TSR[0];
	
	//TBAUD Transmitter baud rate generator
	reg [15:0] TBAUD;
	always @(posedge i_clk)
		if (i_rst) TBAUD <= 0;
		else if (tsr_load | tx_baud) TBAUD <= DL;
		else TBAUD <= TBAUD - 1;

	//DLB Divisor Latch Bit
	reg DLA;	
	always @(posedge i_clk)
		if (i_rst) DLA <=0;
		else if (i_stb & i_we[3] & addr_is3) DLA <= i_dat_w[31];

	wire dll_load = i_stb & i_we[0] & addr_is8;
	wire dlh_load = i_stb & i_we[1] & addr_is9;

	//DL Divisor Latch Value
	reg [15:0] DL;
	always @(posedge i_clk)
		if (i_rst) DL <= 277;//115200 Baud
		else if (dll_load) DL[7:0] <= i_dat_w[7:0];
		else if (dlh_load) DL[15:8] <= i_dat_w[15:8];

	// The Receiver
	reg rx_in;
	always @(posedge i_clk) rx_in <= i_rx;

	//RBaud Receiver baud rate generator
	reg [15:0] RBAUD;
	always @(posedge i_clk)
		if (i_rst) RBAUD <= 0;
		else if (rx_start) RBAUD <= {1'b0,DL[15:1]};
		else if (rx_baud) RBAUD <= DL;
		else RBAUD <= RBAUD - 1;

	//RSR Receiver shifting register
	reg[8:0] RSR;
	always @(posedge i_clk)
		if (rx_idle) RSR <= 9'b111111111;
		else if (rx_baud) RSR <= {rx_in,RSR[8:1]};
	
	wire rx_baud;
	assign rx_baud = (RBAUD == 0);

	reg rx_idle = 1;
	always @(posedge i_clk)
		if (rx_start) rx_idle <= 0;
		else if (rx_stop) rx_idle <= 1;

	wire rx_start;
	assign rx_start = rx_idle & ~rx_in;

	wire rx_stop;
	assign rx_stop = ~rx_idle & ~RSR[0] & rx_baud;

	//RBR Received Byte register
	reg [7:0] RBR;
	always @(posedge i_clk)
		if (i_rst) RBR <= 0;
		else if (rx_stop) RBR <= RSR[8:1];

	reg DR=0;
	always @(posedge i_clk)
		if (rx_stop) DR <=1;
		else if (i_stb & (~|i_we) & addr_is0) DR <= 0;
	
	//output
	wire addr_is0 = ~DLA & (i_addr == 3'd0); //(i_addr == 3'd0); //Receive buffer / Transmit holding register
	wire addr_is1 = ~DLA & (i_addr == 3'd1); // Interrupt enable
	wire addr_is2 =        (i_addr == 3'd2); // Interrupt identification
	wire addr_is3 =        (i_addr == 3'd3); // Line control
	wire addr_is5 =        (i_addr == 3'd5); // Line status
	wire addr_is8 =  DLA & (i_addr == 3'd0); // divisor latch lsb
	wire addr_is9 =  DLA & (i_addr == 3'd1); // divisor latch msb

	assign o_dat_r = i_addr[2] ?
				{16'd0,1'b0,TSE&THE,THE,4'd0,DR,8'd0}: //reg 7 6 5 4
				(DLA? {16'd0,DL}:				   //reg x x 9 8
					  {DLA,7'd0,5'd0,IID2,IID1,IIP,6'd0,TRE,EDA,RBR});    //reg 3 2 1 0

	// interrupt logic
	// Register 1
	reg EDA; // enable received data available
	always @(posedge i_clk)
		if (i_rst) EDA <= 0;
		else if (i_stb & i_we[1] & addr_is1) EDA <= i_dat_w[8]; //addr_is1 is wrong!?

	reg TRE; // Transmit hold register empty
	always @(posedge i_clk)
		if (i_rst) TRE <= 0;
		else if (i_stb & i_we[1] & addr_is1) TRE <= i_dat_w[9];

	//Register 2
	wire IIP = ~(IID1|IID2);// Inverted Interrupt pending (0 - means pending)
	wire IID2 = EDA&DR;
	reg IID1;
	always @(posedge i_clk)
		if (i_rst) IID1 <= 0;
		else if (IID1 & (thr_load |(i_stb & (~|i_we) & addr_is2) )) IID1 <= 0;
		else if (~IID1 & ~THE_old & THE & TRE) IID1 <= 1;

	assign o_int = ~IIP;

endmodule
