module clk_div(clk100m,clk100,clr);
////----------------------------------------//
//`define simulation
//`ifdef simulation
//    parameter MAX_CNT=10;
//`else 
   parameter MAX_CNT=6-1;  //
//`endif
////---------------------------------------//
input clk100m,clr;
output reg clk100;
reg[24:0] cnt;

always@(posedge clk100m or negedge clr)
	begin 
		if(!clr) begin cnt<=0; end
		else if(cnt==MAX_CNT)
		  begin cnt<=0;clk100<=~clk100; end
			  else cnt<=cnt+1; end
endmodule