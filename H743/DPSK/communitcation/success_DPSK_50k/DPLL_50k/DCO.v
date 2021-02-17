module	DCO(
	input					clk,
	input					rst_n,
	
	input					carryPulse,
	input					subtractionPulse,
	input					bothEdge,
	
	output	reg			clk_Para
);

//	50K的参考时钟
parameter	C = 20-1;

reg		[9:0]		count_cnt;
always @ (posedge clk or negedge rst_n)
begin
	if(!rst_n)
		count_cnt <= C;
	else if(carryPulse)
		count_cnt <= count_cnt - 1'b1;
	else if(subtractionPulse)
		count_cnt <= count_cnt + 1'b1;
	else
		count_cnt <= count_cnt;
end

reg	[9:0]		count;
always @ (posedge clk or negedge rst_n)
begin
	if(!rst_n)
		count <= 'd0;
	else if(count >= count_cnt)
		count <= 'd0;
	else if(bothEdge)
		count <= 'd0;
	else 
		count <= count + 1'b1;
end




always @ (posedge clk or negedge rst_n)
begin
	if(!rst_n)
		clk_Para <= 'd1;	
	else if(count < (count_cnt >> 1))
		clk_Para <= 1'b1;
	else
		clk_Para <= 1'b0;
end
endmodule 
 