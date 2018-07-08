`timescale 1ns / 1ps

module XADCdemo(
    input clk,
    input [3:0] sw,
    input vp_in,
    input vn_in,
    output [7:0] data_out,
    output [3:0] led,
    input [3:0] xa_n,
    input [3:0] xa_p,
    output clkslow,
    output ready,
     output MOSI1, SCLK1, SS1, done1,
    output MOSI, SCLK, SS, done
 );
 wire clk_out1;
 clk_wiz_0 instance_name
    (
     // Clock out ports
     .clk_out1(clk_out1),     // output clk_out1
     // Status and control signals
     .reset(reset), // input reset
     .locked(locked),       // output locked
    // Clock in ports
     .clk_in1(clk));      // input clk_in1
   
     wire shift_en,load;
       wire MISO=1;
       wire send=1;
       wire rst=0;
     reg [15:0] data_in=16'b0011100010101010;  
     
       spi_shift shift(
           .clk(clk_out1), 
           .rst(rst), 
           .shift_en(shift_en), 
           .load(load), 
           .shift_in(MISO), 
           .data_in(data_in), 
           .s_out(MOSI)
       );
       
       spi_ctrl ctrl(
           .clk(clk_out1),
           .rst(rst),
           .send(send),
           .shift_en(shift_en),
           .done(done),
           .load(load),
           .SCLK(SCLK),
           .SS(SS)
       );
   

   
    //XADC signals
    wire enable;                     //enable into the xadc to continuosly get data out
    reg [6:0] Address_in = 7'h14;    //Adress of register in XADC drp corresponding to data
   // wire ready;                      //XADC port that declares when data is ready to be taken
    wire [15:0] data;                //XADC data   
    reg [15:0] data0, data1, data2, data3;
//    wire [11:0] shifted_data;
    wire [11:0] shifted_data0, shifted_data1, shifted_data2, shifted_data3;
    
//    reg [32:0] decimal;              //Shifted data to convert to digits
    wire [4:0] channel_out;
    reg [1:0] sel;
    



    ///////////////////////////////////////////////////////////////////
    //XADC Instantiation
    //////////////////////////////////////////////////////////////////
    
    xadc_wiz_0  XLXI_7 (
        .daddr_in    (Address_in), 
        .dclk_in     (clk), 
        .den_in      (enable & |sw), 
        .di_in       (0),
        .dwe_in      (0),
        .busy_out    (),
        .vauxp15     (xa_p[2]),
        .vauxn15     (xa_n[2]),
        .vauxp14     (xa_p[0]),
        .vauxn14     (xa_n[0]),               
        .vauxp7      (xa_p[1]),
        .vauxn7      (xa_n[1]),
        .vauxp6      (xa_p[3]),
        .vauxn6      (xa_n[3]),               
        .do_out      (data),
        .vp_in       (vp_in),
        .vn_in       (vn_in),
        .eoc_out     (enable),
        .channel_out (channel_out),
        .drdy_out    (ready)
    );
                             
    ///////////////////////////////////////////////////////////////////
    //Address Handling Controlled by button
    //////////////////////////////////////////////////////////////////      
    
    always @(sel)      
        case(sel)
        0: Address_in <= 8'h1e;
        1: Address_in <= 8'h17;  
        2: Address_in <= 8'h1f;  
        3: Address_in <= 8'h16;
        default: Address_in <= 8'h14;
        endcase
    always@(negedge ready)
        case (sel)//next select is always next enabled channel, example: sel=0, sw=1001 -> sel=3
        0: sel <= (sw[1] ? 1 : (sw[2] ? 2 : (sw[3] ? 3 : 0)));
        1: sel <= (sw[2] ? 2 : (sw[3] ? 3 : (sw[0] ? 0 : 1)));
        2: sel <= (sw[3] ? 3 : (sw[0] ? 0 : (sw[1] ? 1 : 2)));
        3: sel <= (sw[0] ? 0 : (sw[1] ? 1 : (sw[2] ? 2 : 3)));
        default: sel <= 0;
        endcase
    assign data_out = {ready, 2'b0, channel_out[4:0]};
    always@(posedge ready) begin
        case (sel)
        0: data0 <= (channel_out == 8'h1E) ? data : data0;
        1: data1 <= (channel_out == 8'h17) ? data : data1;
        2: data2 <= (channel_out == 8'h1F) ? data : data2;
        3: data3 <= (channel_out == 8'h16) ? data : data3;
        endcase
    

    end
    ///////////////////////////////////////////////////////////////////
    //LED PWM
    //////////////////////////////////////////////////////////////////  
    
    integer pwm_end = 4070;
    //filter out tiny noisy part of signal to achieve zero at ground
    assign shifted_data0 = (data0 >> 4) & 12'hff0;
    assign shifted_data1 = (data1 >> 4) & 12'hff0;
    assign shifted_data2 = (data2 >> 4) & 12'hff0;
    assign shifted_data3 = (data3 >> 4) & 12'hff0;

    integer pwm_count = 0;
//    reg pwm_out = 0;
   

    //Pwm the data to show the voltage level
    always @(posedge(clk))begin
        if(pwm_count < pwm_end)begin
            pwm_count = pwm_count+1;
        end           
        else begin
            pwm_count=0;
        end
    end
    //leds are active high
    assign led[0] = (sw[0] == 1'b0) ? 1'b0 : (pwm_count < shifted_data0 ? 1'b1 : 1'b0);
    assign led[1] = (sw[1] == 1'b0) ? 1'b0 : (pwm_count < shifted_data1 ? 1'b1 : 1'b0);
    assign led[2] = (sw[2] == 1'b0) ? 1'b0 : (pwm_count < shifted_data2 ? 1'b1 : 1'b0);
    assign led[3] = (sw[3] == 1'b0) ? 1'b0 : (pwm_count < shifted_data3 ? 1'b1 : 1'b0);
    
    
// CREATION OF slower clock for SPI communication with Arduino  
         reg [10:0] subclk='b0;
  reg[0:0] sub='b0;

  assign clkslow =sub;
     always @(posedge clk_out1) begin
     subclk <= subclk+1'b1;
     if(subclk==100) begin
      if(sub==0)
      sub <=1'b1;
      else
      sub <=1'b0;
      subclk <='b0;
      end
      end

wire [7:0]data_memory;
reg [7:0] data_in1;

wire shift_en1,load1;

wire send_memory;
reg send1=1;

spi_shift1 shift1(
         .clk(sub), 
         .rst(rst), 
         .shift_en(shift_en1), 
         .load(load1), 
         .shift_in(MISO), 
         .data_in(data_in1), 
         .s_out(MOSI1)
     );
     
     wire SCLK_d;
     
     spi_ctrl1 ctrl1(
         .clk(sub),
         .rst(rst),
          .send(send1),
         .shift_en(shift_en1),
         .done(done1),
         .load(load1),
         .SCLK(SCLK_d),
         .SS(SS1)
     );


// Delay module for SPI communication with Arduino
delay delay (
.clk_27(clk_out1),
.vsync(SCLK_d),
.vsync_o(SCLK1)
);
     
     
     
    
         parameter valor1 = 2'b00;
      parameter valor2 = 2'b01;
      parameter valor3 = 2'b10;
      parameter valor4 = 2'b11;
   
      reg [1:0] valor_actual = valor1;
   
   reg [16:0] counter='b0;
 reg [15:0] soma='b0;
 reg [5:0] media='b0;

 reg [11:0] datafilt;
 reg [1:0] filter_finished='b0;
 
 // FILTER Implemented
 always@(posedge ready) begin
          if(media<16)begin
          soma <=soma +data0[15:4]; // Adding of 16 values
          media <= media+1'b1;
          end
          else begin
          datafilt[11:0]=soma >> 4;// 4bit shift
          soma <= 0;
          media<=0;
          filter_finished <=1'b1;
    
          end
          end
 
 wire [11:0] datafilt_memory;
 wire [15:0] data_test;

 assign data_test[11:0]=datafilt[11:0];
 assign data_test[15:12]='b0;
 assign datafilt_memory=datafilt[11:0];
 wire filter_finished_memory;
 assign filter_finished_memory=filter_finished;
 reg[0:0] slow='b0;
 
 
      always @(posedge clk_out1)begin
    
        if (sw[0]==1'b1) begin // IF SWITCH IS TURNED ON DAC OUTPUT IS PROPORTIONAL TO ADC INPUT
            valor_actual <= valor1;
   
          data_in[15:13]=3'b001;
      
      if(data0[15:4]<=4'b0001)
        data_in[12:1]=12'b111111111111;
       else   begin
 
             data_in[12:1]=datafilt[11:0];

              data_in1[7:0]=data_memory;
              send1=send_memory;
        end
         end
         
           else begin

           counter <= counter+1'b1;// IF SWITCH TURNED OFF DAC OUTPUT IS CONTROLLED BY A STATE MACHINE
            case (valor_actual )
               valor1 : begin
                  if (counter[16:15]==2'b00) begin
                      data_in =16'b0010000000000000;  
                     valor_actual <= valor2; end
                    
              
               end
              valor2 : begin
                  if (counter[16:15]==2'b01) begin
                   data_in =16'b0010100000000000;
                  valor_actual <= valor3; end
        
 
            
               end
               valor3 : begin
                  if (counter[16:15]==2'b10) begin
                    data_in =16'b0011000000000000;
                    valor_actual <= valor4; end
    
           
               end
               valor4 : begin
                  if (counter[16:15]==2'b11) begin
                data_in =16'b0011100000000000;
                   valor_actual <= valor1; end
             
                        
               end
               
               endcase
               end
      end
      
   // MEMORY FIFO
   
    //  assign dad=datafilt;
      reg [2:0] read_clk_aux;
      reg [1:0] read_clk='b0;
       always @(posedge sub) begin
          read_clk_aux <= read_clk_aux+1'b1;
          if(subclk==9) begin
           if(read_clk==0)begin
           read_clk <=1'b1;
  //      data_in1 <=data_memory;
    //     send1<=send_memory;
           end
           else
           read_clk <=1'b0;
           read_clk_aux <='b0;
           end
           
           end

               
               memory memory(
               .rst(rst),     
               .clk_out1(clk_out1),
               .datafilt(/*data_test*/'hC49F),   
               .filter_finished1(filter_finished_memory),   
              .done(done1),
               .read_clk(/*read_clk*/sub),   
                .data_in2(data_memory),   
                .full(),
                .send2(send_memory),   
                .empty()   
               );
         
       
endmodule
