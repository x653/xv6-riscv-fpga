/*
CLINT - Core Local Interrupt controller

Provides memory mapped registers to generate timer and software interrupts.
Base address: 0x02000000

msip:     Generates machine mode software interrupts when set.
          Only msip[0] is implemented msip[31:1] are always 0.
mtimecmp: Compare value for timer interrupt.
          Writing to register clears the timer interrupt.
mtime:    Current time value.
          Ticks are clocked by system clock.

register    offset    size       reset     
-----------------------------------------
msip        0x0000    32 bits    0x00      
mtimecmp    0x4000    64 bits    0x0000
mtime       0xBFF8    64 bits    0x0000

*/
`default_nettype none

module clint(
	input wire i_clk,
	input wire i_rst,
	input wire [15:0] i_addr,
	input wire [3:0] i_we,
	output wire [31:0] o_dat_r,
	input wire [31:0] i_dat_w,
	input wire i_stb,
	output wire o_ack,
	output wire o_timer_int,
	output wire o_software_int
);

//CLINT reads/writes execute immediately (no delay)
assign o_ack = i_stb;

//memory offsets
wire addr_is_msip       = (i_addr==16'h0000);
wire addr_is_mtimecmp_l = (i_addr==16'h4000);
wire addr_is_mtimecmp_h = (i_addr==16'h4004);
wire addr_is_mtime_l    = (i_addr==16'hBFF8);
wire addr_is_mtime_h    = (i_addr==16'hBFFC);

//msip register
reg msip;
always @(posedge i_clk)
	if (i_rst) msip <= 0;
	else if (addr_is_msip & i_stb & i_we[0]) msip <= i_dat_w[0];

//mtimecmp register holds compare value to generate interrupts
reg [63:0] mtimecmp;
always @(posedge i_clk)
	if (i_rst) mtimecmp <= 64'h7fffffff_ffffffff;
	else if (addr_is_mtimecmp_l) begin
		if (i_stb & i_we[0]) mtimecmp[7:0]   <= i_dat_w[7:0];
		if (i_stb & i_we[1]) mtimecmp[15:8]  <= i_dat_w[15:8];
		if (i_stb & i_we[2]) mtimecmp[23:16] <= i_dat_w[23:16];
		if (i_stb & i_we[3]) mtimecmp[31:24] <= i_dat_w[31:24];
	end
	else if (addr_is_mtimecmp_h) begin
		if (i_stb & i_we[0]) mtimecmp[39:32] <= i_dat_w[7:0];
		if (i_stb & i_we[1]) mtimecmp[47:40] <= i_dat_w[15:8];
		if (i_stb & i_we[2]) mtimecmp[55:48] <= i_dat_w[23:16];
		if (i_stb & i_we[3]) mtimecmp[63:56] <= i_dat_w[31:24];
	end

//mtime register holds 64 bit time value
reg [63:0] mtime;
always @(posedge i_clk)
	if (i_rst) mtime <= 0;
	else  mtime <= mtime + 1;

//output on read	
assign o_dat_r = (addr_is_msip?          {31'd0,msip}: 0)|
				 (addr_is_mtimecmp_l?  mtimecmp[31:0]: 0)|
				 (addr_is_mtimecmp_h? mtimecmp[63:32]: 0)|
				 (addr_is_mtime_l?        mtime[31:0]: 0)|
				 (addr_is_mtime_h?       mtime[63:32]: 0);

//generate interrupts
assign o_timer_int    = (mtime >= mtimecmp);
assign o_software_int = msip;

endmodule
