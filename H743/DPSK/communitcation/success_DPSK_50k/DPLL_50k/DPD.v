module	DPD(
	input					clk,
	input					rst_n,
	
	input					M_Data,//输入信号
	input					clk_Para,//本地参考信号
	
	output					bothEdge,//携带输入信号边沿信息的脉冲序列
	output	reg			sign_hou,//滞后信号
	output	reg			sign_qian//超前信号
	
);

reg				Q;
always @ (posedge clk or negedge rst_n)
begin
	if(!rst_n)
		Q <= 'd0;
	else 
		Q <= M_Data;
end
//	携带输入信号边沿的脉冲序列
assign	bothEdge = M_Data ^ Q;

always @ (posedge clk or negedge rst_n)
begin
	if(!rst_n)	begin
		sign_hou <= 'd0;
		sign_qian <= 'd0;
	end
	else if(bothEdge == 1'b1 && clk_Para == 1'b0)
		sign_hou <= 1'b1;
	else if(bothEdge == 1'b1 && clk_Para == 1'b1)
		sign_qian <= 1'b1;
	else begin
		sign_hou <= 'd0;
		sign_qian <= 'd0;
	end
end
endmodule
 