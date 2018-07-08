`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 06/29/2018 11:54:56 AM
// Design Name: 
// Module Name: delay
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


module delay( 
clk_27,
vsync,
vsync_o
);
input clk_27;
input wire vsync;
output reg vsync_o;

reg[112:0] bits; //Delay for 112 clk cycles corresponding to about 1 SPI clk cycle

always @ (posedge clk_27)
begin
    vsync_o <= bits[112];
    bits <= {bits[111:0], vsync};
end //always

endmodule