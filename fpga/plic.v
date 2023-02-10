/*
PLIC - Platform Level Interrupt Controller

Provides memory mapped registers to generate platform level interrupts.
Base address: 0x0C000000

Only partially implemented, as required by xv6!

pending   Interrupt pending bit 0-31
enable_m  Enable bits for sources 0-31 on context 0
enable_s  Enable bits for sources 0-31 on context 1
claim_m   Claim/complete for context 0
claim_s   Claim/complete for context 1

register  offset      size       reset     
--------------------------------------------
pending   0x001000    32 bits    0x00      
enable_m  0x002000    32 bits    0x00
enable_s  0x002080    32 bits    0x00
claim_m   0x200004    32 bits    0x00
claim_s   0x201004    32 bits    0x00
*/
`default_nettype none

module plic(
	input wire i_clk,
	input wire i_rst,
	input wire [23:0] i_addr,
	input wire [3:0] i_we,
	output wire [31:0] o_dat_r,
	input wire [31:0] i_dat_w,
	input wire i_stb,
	input wire [31:1] i_int,
	output wire o_ack,
	output wire o_ext_int_m,
	output wire o_ext_int_s
);

// plic read/write need no delay and acknoledge immediately
assign o_ack = i_stb;

// register offset
wire addr_is_pending     = (i_addr == 24'h001000);
wire addr_is_enable_m    = (i_addr == 24'h002000);
wire addr_is_enable_s    = (i_addr == 24'h002080);
wire addr_is_claim_m     = (i_addr == 24'h200004);
wire addr_is_claim_s     = (i_addr == 24'h201004);

// enable interrupt on context 0 (memory mode)
reg [31:0] enable_m;
always @(posedge i_clk) begin
	if (i_rst) enable_m <= 0;
	else if (addr_is_enable_m) begin
		if (i_stb & i_we[0]) enable_m[7:0]   <= i_dat_w[7:0];
		if (i_stb & i_we[1]) enable_m[15:8]  <= i_dat_w[15:8];
		if (i_stb & i_we[2]) enable_m[23:16] <= i_dat_w[23:16];
		if (i_stb & i_we[3]) enable_m[31:24] <= i_dat_w[31:24];
	end
end

// enable interrupt on context 1 (supervisor mode)
reg [31:0] enable_s;
always @(posedge i_clk) begin
	if (i_rst) enable_s <= 0;
	else if (addr_is_enable_s) begin
		if (i_stb & i_we[0]) enable_s[7:0]   <= i_dat_w[7:0];
		if (i_stb & i_we[1]) enable_s[15:8]  <= i_dat_w[15:8];
		if (i_stb & i_we[2]) enable_s[23:16] <= i_dat_w[23:16];
		if (i_stb & i_we[3]) enable_s[31:24] <= i_dat_w[31:24];
	end
end

// interrupt pending on context 0 (machine mode)
reg [31:0] pending_m;
always @(posedge i_clk)
	if (i_rst) pending_m <= 0;
	else if (i_stb & (|i_we) & addr_is_claim_m) pending_m <= pending_m & ~(1<<i_dat_w[7:0]);
	else pending_m <= pending_m | ({i_int,1'b0} & enable_m);

// interrupt pending con context 1 (supervisor mode)
reg [31:0] pending_s;
always @(posedge i_clk)
	if (i_rst) pending_s <= 0;
	else if (i_stb & (|i_we) & addr_is_claim_s) pending_s <= pending_s & ~(1<<i_dat_w[7:0]);
	else pending_s <= pending_s | ({i_int,1'b0} & enable_s);

// read register
assign o_dat_r = (addr_is_pending?	  	 pending_m|pending_s   : 32'd0)|
				 (addr_is_enable_m?      enable_m    : 32'd0)|
				 (addr_is_enable_s?      enable_s    : 32'd0)|
				 (addr_is_claim_m?       claim_m_bin : 32'd0)|
				 (addr_is_claim_s?       claim_s_bin : 32'd0);

// claim interrupt on context 0 (machine mode)
wire [31:0] claim_m = pending_m & -pending_m;
wire [31:0] claim_m_bin = {27'd0,|(claim_m & 32'hFFFF0000),
						         |(claim_m & 32'hFF00FF00),
						         |(claim_m & 32'hF0F0F0F0),
                                 |(claim_m & 32'hCCCCCCCC),
                                 |(claim_m & 32'hAAAAAAAA)};

// claim interrupt on context 1 (supervisor mode)
wire [31:0] claim_s= pending_s & -pending_s;
wire [31:0] claim_s_bin = {27'd0,|(claim_s & 32'hFFFF0000),
						         |(claim_s & 32'hFF00FF00),
						         |(claim_s & 32'hF0F0F0F0),
                                 |(claim_s & 32'hCCCCCCCC),
                                 |(claim_s & 32'hAAAAAAAA)};

// signal interrupt to CPU
assign o_ext_int_m = |pending_m;
assign o_ext_int_s = |pending_s;

endmodule
