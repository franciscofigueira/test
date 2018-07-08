`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/17/2018 03:00:12 PM
// Design Name: 
// Module Name: memory
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module memory(
input wire rst,
input wire clk_out1,
input wire  [15:0]  datafilt,
input wire filter_finished1,
input wire done,
input wire read_clk,
output wire [7:0] data_in2,
output full,
output empty,
output wire send2
    );
    reg [7:0] data_in1;
    reg send1;
    assign send2=send1;
    assign data_in2=data_in1;
wire[7:0] dout;
reg  [0:0]rd_en;

reg filter_finished;

wire [15:0] dados1;
reg [15:0] dados;
assign dados1=dados;

           
           reg [0:0] wr_en;
          // reg [0:0] rd_en;
          // wire [7:0] dout;

 fifo_generator_0 your_instance_name (
                   .rst(rst),                  // input wire rst
                   .wr_clk(clk_out1),            // input wire wr_clk
                   .rd_clk(read_clk),            // input wire rd_clk
                   .din(dados1),                  // input wire [15 : 0] din
                   .wr_en(wr_en),              // input wire wr_en
                   .rd_en(rd_en),              // input wire rd_en
                   .dout(dout),                // output wire [7 : 0] dout
                   .full(full),                // output wire full
                   .empty(empty),              // output wire empty
                   .wr_rst_busy(wr_rst_busy),  // output wire wr_rst_busy
                   .rd_rst_busy(rd_rst_busy)  // output wire rd_rst_busy
                 );
                 
                             parameter write_stat =2'b01;
                 parameter read_stat=2'b10;
                 reg [1:0] estado_actual=write_stat;
                 reg[20:0] read='b0;
                 reg debug_r=0;
            always @(posedge clk_out1) begin
            filter_finished <=filter_finished1;
               case (estado_actual)
               write_stat: begin
              
               if(filter_finished==1'b1)begin // wait until data aquisition and filter finished
                   wr_en<=1'b1;
               dados <= datafilt;
               filter_finished <= 1'b0;
                  end
               else
               wr_en<=1'b0;
               
               if(full==1'b1)begin
               estado_actual <= read_stat;
               wr_en<=1'b0;
               debug_r <= !debug_r;
               end
               
                end        
     
               read_stat:begin
    
               if(done==1'b1)begin //wait until SPI transmission is complete
               rd_en<=1'b1;
               data_in1<=dout;
               send1 <=1'b1;
               end
               else begin
               rd_en <=1'b0;
               send1 <=1'b0;
              end
               
               if(empty)begin
               estado_actual <= write_stat;
               rd_en<=1'b0;
               send1 <=1'b0;
               debug_r<=!debug_r;
               end
               end
               endcase
      
      end
      
      endmodule