
`default_nettype none
module soc(
	input	i_clk,
	input	i_rst,
	input	i_uart_rx,
	output	o_uart_tx,
	output [17:0]	o_sram_a,
	inout  [15:0]	io_sram_d,
	output	o_sram_csn,
	output	o_sram_oen,
	output	o_sram_wen,
	output [1:0] o_led,
	input i_spi_miso,
	output o_spi_mosi,
	output o_spi_sck,
	output o_spi_ss
);

parameter RAM_SIZE = 16384-4096;

// SBA Simple Bus Architecture
wire		sba_rst = i_rst;
wire		sba_clk = i_clk;
wire  [3:0] sba_we;
wire        sba_stb;
wire [31:0] sba_addr;
wire [31:0] sba_dat_r;
wire [31:0] sba_dat_w;
wire        sba_ack;

// SBA Address space
// ----------------------------------
// BRAM  00000000 - 00002000 (8 kb)
// CLINT 02000000
//       02004000 - MTIMECMP
//       0200BFF8 - MTIME
// PLIC  0C000000
// UART  10000000 - 10000007 (8 b)
// SPI   10001000
// SRAM  80000000 - 80080000 (512 kb)
wire addr_is_bram =  (sba_addr[31:24]==8'h00);
wire addr_is_clint = (sba_addr[31:24]==8'h02);
wire addr_is_plic  = (sba_addr[31:24]==8'h0C);
wire addr_is_uart =  (sba_addr[31:12]==20'h10000);
wire addr_is_spi  =  (sba_addr[31:12]==20'h10001);
wire addr_is_sram =  (sba_addr[31:24]==8'h80);

assign sba_ack = addr_is_bram ? bram_ack:
				 addr_is_uart ? uart_ack:
				 addr_is_sram ? sram_ack:
				 addr_is_clint ? clint_ack:
				 addr_is_plic ? plic_ack:
				 addr_is_spi ? spi_ack:
				 1'd0;
    
assign sba_dat_r =	addr_is_bram ? bram_dat_r:
				 	addr_is_uart ? uart_dat_r:
				 	addr_is_sram ? sram_dat_r:
				 	addr_is_clint ? clint_dat_r:
				 	addr_is_plic ? plic_dat_r:
				 	addr_is_spi ? spi_dat_r:
				 	32'd0;

wire [1:0] rv_mode;
assign o_led = {rv_mode[1],rv_mode[0]};
// rv32i risc-v cpu
rv32 RV32(
    .i_rst(sba_rst),
	.i_clk(sba_clk),			
	.o_addr(sba_addr),
	.o_dat_w(sba_dat_w),
	.o_we(sba_we),
	.i_dat_r(sba_dat_r),
	.o_led(rv_mode),
	.o_stb(sba_stb),
	.i_ack(sba_ack),
    .i_mtip(clint_timer_int),
    .i_msip(clint_software_int),
	.i_meip(plic_ext_int_m),
	.i_seip(plic_ext_int_s)
);
   
// BRAM 8 kb of ram preloaded with firmware   
reg [31:0] BRAM[0:(RAM_SIZE/4)-1];
initial $readmemh("firmware.hex",BRAM); 
wire bram_stb = addr_is_bram & sba_stb;
wire [11:0] bram_addr = sba_addr[13:2];
always @(posedge i_clk) begin
	if(sba_we[0] & bram_stb) BRAM[bram_addr][ 7:0 ] <= sba_dat_w[ 7:0 ];
	if(sba_we[1] & bram_stb) BRAM[bram_addr][15:8 ] <= sba_dat_w[15:8 ];
	if(sba_we[2] & bram_stb) BRAM[bram_addr][23:16] <= sba_dat_w[23:16];
	if(sba_we[3] & bram_stb) BRAM[bram_addr][31:24] <= sba_dat_w[31:24];
end
reg [31:0] bram_dat_r;
always @(posedge i_clk)
	bram_dat_r = BRAM[bram_addr];
//wire [31:0] bram_dat_r = BRAM[bram_addr];
reg bram_ack;
always @(posedge i_clk)
	if (bram_stb) bram_ack <= 1;
	else bram_ack <= 0;
//wire bram_ack = bram_stb;

//CLINT
wire [31:0] clint_dat_r;
wire clint_ack;
wire clint_timer_int;
wire clint_software_int;
clint CLINT(
	.i_clk(sba_clk),
	.i_rst(sba_rst),
	.i_addr(sba_addr[15:0]),
	.i_we(sba_we),
	.i_dat_w(sba_dat_w),
	.i_stb(addr_is_clint & sba_stb),
	.o_dat_r(clint_dat_r),
	.o_ack(clint_ack),
	.o_timer_int(clint_timer_int),
	.o_software_int(clint_software_int)
);

//PLIC
wire [31:0] plic_dat_r;
wire plic_ack;
wire plic_ext_int_s;
wire plic_ext_int_m;
plic PLIC(
	.i_clk(sba_clk),
	.i_rst(sba_rst),
	.i_addr(sba_addr[23:0]),
	.i_we(sba_we),
	.i_dat_w(sba_dat_w),
	.i_stb(addr_is_plic & sba_stb),
	.i_int({21'd0,uart_int,9'd0}),
	.o_dat_r(plic_dat_r),
	.o_ack(plic_ack),
	.o_ext_int_m(plic_ext_int_m),
	.o_ext_int_s(plic_ext_int_s)
);

// SRAM 512 kb
wire [31:0] sram_dat_r;
wire sram_ack;

sram SRAM(
	.i_clk(sba_clk),
	.i_rst(sba_rst),
	.io_sram_d(io_sram_d),
	.o_sram_a(o_sram_a),
	.o_sram_csn(o_sram_csn),
	.o_sram_oen(o_sram_oen),
	.o_sram_wen(o_sram_wen),
	.i_addr(sba_addr[21:2]),
	.o_dat_r(sram_dat_r),
	.i_dat_w(sba_dat_w),
	.i_stb(addr_is_sram & sba_stb),
	.i_we(sba_we),
	.o_ack(sram_ack)
);
  
// UART
wire [31:0] uart_dat_r;
wire uart_ack;
wire uart_int;
uart UART(
	.i_clk(sba_clk),
	.i_rst(sba_rst),
	.i_stb(addr_is_uart & sba_stb),
	.i_we(sba_we),
	.o_ack(uart_ack),
	.i_addr(sba_addr[2:0]),
	.i_dat_w(sba_dat_w),
	.o_dat_r(uart_dat_r),
	.o_tx(o_uart_tx),
	.i_rx(i_uart_rx),
	.o_int(uart_int)
);
//SPI (SD Card)
wire [31:0] spi_dat_r;
wire spi_ack;
spi SPI(
	.i_clk(sba_clk),
	.i_rst(sba_rst),
	.i_addr(sba_addr[3:0]),
	.i_stb(addr_is_spi & sba_stb),
	.i_we(sba_we[0]),
	.o_ack(spi_ack),
	.i_dat_w(sba_dat_w),
	.o_dat_r(spi_dat_r),
	.i_miso(i_spi_miso),
	.o_mosi(o_spi_mosi),
	.o_sck(o_spi_sck),
	.o_ss(o_spi_ss)
);
endmodule
