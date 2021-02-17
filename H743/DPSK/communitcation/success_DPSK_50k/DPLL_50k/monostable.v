module monostable (
	clk32_i	,
	rst_n_i	,
	data_i	,
	data_o	
);
	input	wire		clk32_i	;
	input	wire		rst_n_i	;
	input	wire		data_i	;
	output	reg		data_o	;
	
	reg	data_i_reg1 , data_i_reg2 ,data_i_reg3 ,data_i_reg4 ;
	
	always @(posedge clk32_i or negedge rst_n_i)
	begin
		if(!rst_n_i)
			data_o 	<= 1'b0 ;
		else	if(data_i_reg4)
			data_o	<= 1'b0	;
		else	if(data_i)
			data_o	<= 1'b1	;
		else
			;
	end	
	
	always @(posedge clk32_i or negedge rst_n_i)
	begin
		if(!rst_n_i)begin
			data_i_reg1	<= 1'b0 ;
			data_i_reg2	<= 1'b0 ;
			data_i_reg3	<= 1'b0 ;
			data_i_reg4	<= 1'b0 ;end
		else	begin	
			data_i_reg1	<= data_i	;
			data_i_reg2	<= data_i_reg1;
			data_i_reg3	<= data_i_reg2;
			data_i_reg4	<= data_i_reg3;end
	end
	
endmodule 
