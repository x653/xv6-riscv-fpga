/*
SPI - Serial Peripherial Interface

Provides memory mapped registers to control SPI connection to SD-Card
Base address: 0x10001000

RW:     Write to RW to send 8 bits to SDO with the appropriate SCK pulses
        Read from RW the last byte received at SDI
CTR:    Control the control wires:
        CTR[0] = CSN: Chip Select not
		CTR[1] = SCK: serial clock

register  offset  size       reset     
-----------------------------------------
RW        0x00    8 bits    0x00      
CTR       0x04    8 bits    0x00
*/
`default_nettype none

module spi(
	input wire i_clk,
	input wire i_rst,
	input wire i_stb,
	input wire i_we,
	input wire [31:0] i_dat_w,
	input wire [3:0] i_addr,
	output wire [31:0] o_dat_r,
	output reg o_ack,
	output reg o_ss,
	output wire o_mosi,
	input wire i_miso,
	output wire o_sck
);

	wire start;
	assign start = ready & ~i_addr[2] & i_we & i_stb;
	always @(posedge i_clk)
		if (i_rst) o_ss <= 1;
		else if (i_stb & i_addr[2] & i_we) o_ss <= i_dat_w[0];
	
	reg run;
	wire ready;
	assign ready = ~run;
	always @(posedge i_clk)
		if (i_rst) run <= 0;
		else if (~run & start) run <= 1;
		else if (run & stop) run <=0;

	reg [3:0] bits;
	always @(posedge i_clk)
		if (i_rst | start) bits <= 0;
		else if (i_stb & i_addr[2] & i_we) bits[0] <= i_dat_w[1];
		else if (run) bits <= bits + 1; 

	assign o_sck = bits[0];
	always @(posedge i_clk)
	 	if (i_rst) o_ack <= 0;
		else if (stop | (i_stb & ((~i_addr[2] & ~i_we)|i_addr[2] ))) o_ack <= 1;	
		else o_ack <= 0;

	wire sample;
	assign sample = run & ~o_sck;
	reg inLSB;
	always @(posedge i_clk)
		if (i_rst) inLSB <= 0;
		else if (sample) inLSB <= i_miso;

	wire shift;
	assign shift=run&o_sck;	
	reg [7:0] data;
	always @(posedge i_clk)
		if (i_rst) data <= 0;
		else if (start) data <= i_dat_w[7:0];
		else if (shift) data <= {data[6:0],inLSB};

	assign o_mosi = data[7];
	
	wire stop;
	assign stop = bits[3]&bits[2]&bits[1]&o_sck;

	assign o_dat_r = i_addr[2]? {31'd0,o_ss}:{24'b0000000,data};

endmodule
