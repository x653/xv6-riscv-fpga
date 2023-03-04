module soc_tb();
//reset
reg rst=0;
reg clk=0;
wire tx;

uart UART(.i_clk(clk),.i_rst(rst),.i_rx(tx));

soc SOC(
    .i_clk(clk),
	.i_rst(rst),
    .o_uart_tx(tx)
);
wire spi;

always #1 clk = ~clk;


initial begin
  $dumpfile("soc_tb.vcd");
  $dumpvars(0, soc_tb);
	#0 rst = 1;
	#20 rst = 0;
   # 80000;
$finish;
end

endmodule
