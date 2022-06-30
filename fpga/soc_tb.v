module soc_tb();
//reset
reg [3:0]	rst_cnt = 0;
wire		rst = ! (& rst_cnt);
always @(posedge clk) rst_cnt <= rst_cnt + {15'd0,rst};

localparam BAUD = 173;
localparam BITRATE = (BAUD << 1);
localparam FRAME = (BITRATE * 11);
localparam FRAME_WAIT = (BITRATE * 4);
task send_car;
input [7:0] car;
begin
    rx <= 0;                 //-- Bit start 
    #BITRATE rx <= car[0];   //-- Bit 0
    #BITRATE rx <= car[1];   //-- Bit 1
    #BITRATE rx <= car[2];   //-- Bit 2
    #BITRATE rx <= car[3];   //-- Bit 3
    #BITRATE rx <= car[4];   //-- Bit 4
    #BITRATE rx <= car[5];   //-- Bit 5
    #BITRATE rx <= car[6];   //-- Bit 6
    #BITRATE rx <= car[7];   //-- Bit 7
    #BITRATE rx <= 1;        //-- Bit stop
    #BITRATE rx <= 1;        //-- Esperar a que se envie bit de stop
end
endtask
//wire rst;
reg clk = 0;
wire tx;
reg rx = 1;
reg [15:0] but;
wire [15:0] sram_d;
wire [17:0] sram_a;
assign sram_d = (sram_oen)? 16'bzzzzzzzzzzzzzzzz: (sram_a==0)? 16'h4241: (sram_a==1)? 16'h4443 : (sram_a==2)? 16'hdead: 16'hbeaf;
wire sram_oen;
uart UART(.i_clk(clk),.i_rst(rst),.i_rx(tx));
soc SOC(
    .i_clk(clk),
	.i_rst(rst),
    .o_uart_tx(tx),
    .i_uart_rx(rx),
    .io_sram_d(sram_d),
    .o_sram_a(sram_a),
	.o_sram_oen(sram_oen),
	.o_spi_mosi(spi),
	.i_spi_miso(spi)
);
wire spi;

always #1 clk = ~clk;


initial begin
  $dumpfile("soc_tb.vcd");
  $dumpvars(0, soc_tb);
	//#2 rst = 1;
	//#2 rst = 0;
	#100 but = 16'd0;
	#100 but = 16'd1;
	#100 but = 16'd2;
	#100 but = 16'd3;
	#100 but = 16'd0;
	#100 but = 16'd0;
	#100 but = 16'd0;
   #(FRAME_WAIT * 10) send_car(" ");
   #(FRAME_WAIT * 10) send_car(" ");
   #(FRAME_WAIT * 10) send_car("0");
   #(FRAME_WAIT * 10) send_car("x");
   #(FRAME_WAIT * 10) send_car("c");
   #(FRAME_WAIT * 10) send_car(" ");
   #(FRAME_WAIT * 10) send_car("e");
   #(FRAME_WAIT * 10) send_car("1");
   #(FRAME_WAIT * 10) send_car("1");
   #(FRAME_WAIT * 10) send_car("2");
   #(FRAME_WAIT * 10) send_car("3");
   #(FRAME_WAIT * 10) send_car("3");
   #(FRAME_WAIT * 10) send_car("3");
   #(FRAME_WAIT * 10) send_car(10);
   #(FRAME_WAIT * 10) send_car(10);
   #(FRAME_WAIT * 10) send_car(10);
   #(FRAME_WAIT * 10) $display("FIN de la simulacion");
   # 8000;
$finish;
end

endmodule
