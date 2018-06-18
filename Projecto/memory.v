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
input dados,
input filter_finished,
input rd_en,
input wire read_clk,
output dout,
output full,
output empty,
output send1
    );
endmodule

     wire full;
               wire empty;
      reg [15:0] dados;
    //  assign dad=datafilt;
      reg [2:0] read_clk_aux;
      reg [1:0] read_clk='b0;
 
           
           reg [0:0] wr_en;
           reg [0:0] rd_en;
           wire [7:0] dout;

 fifo_generator_0 your_instance_name (
                   .rst(rst),                  // input wire rst
                   .wr_clk(clk_out1),            // input wire wr_clk
                   .rd_clk(read_clk),            // input wire rd_clk
                   .din(dados),                  // input wire [15 : 0] din
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
             always @(posedge clk_out1) begin
               case (estado_actual)
               write_stat: begin
               
               if(filter_finished==1'b1)begin
                   wr_en<=1'b1;
               dados <= datafilt;
               filter_finished <= 1'b0;
                  end
               else
               wr_en<=1'b0;
               
               if(full==1'b1)begin
               estado_actual <= read_stat;
               wr_en<=1'b0;
               end
               
                end        
     
               read_stat:begin
               read <= read+1'b1;
                if(read >=190)begin
                      data_in1<=dout;
                      read <= 'b0;
                end 
                      
               rd_en=1'b1;
               send1 <=1'b1;
               
              
               
               if(empty)begin
               estado_actual <= write_stat;
               rd_en<=1'b0;
               send1 <=1'b0;
               end
               end
               endcase
      
      end