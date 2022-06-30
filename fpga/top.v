`default_nettype none
module top(
   input wire clk,
   input wire uart_rx,
   output wire uart_tx,
   inout [15:0] sram_d,
   output [17:0] sram_a,
   output wire sram_wen,
   output wire sram_oen,
   output wire sram_csn,
   output [1:0] led,
   input [1:0] but,
   input sd_miso,
   output sd_mosi,
   output sd_sck,
   output sd_ss
);

//reset
reg [15:0]	rst_cnt = 0;
wire		rst = ! (& rst_cnt);
always @(posedge o_clk) rst_cnt <= rst_cnt + {15'd0,rst};

//PLL   
wire  o_clk;
pll PLL(.clock_in(clk),.clock_out(o_clk));

//SOC
soc SOC(
	.i_clk(o_clk),
	.i_rst(rst),
	.o_uart_tx(uart_tx),
	.i_uart_rx(uart_rx),
	.o_led(led),
	.io_sram_d(sram_d),
	.o_sram_a(sram_a),
	.o_sram_csn(sram_csn),
	.o_sram_oen(sram_oen),
	.o_sram_wen(sram_wen),
	.i_spi_miso(sd_miso),
	.o_spi_mosi(sd_mosi),
	.o_spi_sck(sd_sck),
	.o_spi_ss(sd_ss)
);
   
endmodule
