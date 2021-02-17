module differpd(	
	rst_n_i	,
	clk32_i,
	data_i,
	clk_i_i,
	clk_q_i,
	pd_bef_o,
	pd_aft_o
);
	input		wire			rst_n_i	;
	input		wire			clk32_i	;
	input		wire			data_i	;
	input		wire			clk_i_i	;
	input		wire			clk_q_i	;
	output	reg				pd_bef_o;
	output	reg				pd_aft_o;
	
	reg		data_reg0	;
	wire	data_edge	;
	
	always @(posedge clk32_i or negedge rst_n_i)
	begin
		if(!rst_n_i)
			data_reg0	<= 1'b0 ;
		else
			data_reg0	<= data_i	;
	end
	
	assign	data_edge = data_i	^ data_reg0 ;
	
	always @(posedge clk32_i or negedge rst_n_i)
	begin
		if(!rst_n_i)begin
			pd_bef_o	<= 1'b0 ;
			pd_aft_o	<= 1'b0	;end
		else	begin
			pd_bef_o	<= (data_edge & clk_i_i)		;
			pd_aft_o	<= (data_edge	&	clk_q_i)		;end
	end
	
endmodule 

