module main(
	clk_12	,
	rst_n_i	,
	data_i	,
	syn_o	,
	clk1
);
	input		wire			clk_12	;
	input		wire			rst_n_i	;
	input		wire			data_i	;
	output	wire			syn_o		;
	output	wire			clk1		;

//	wire   clk4
	wire	clk_d1	, clk_d2	;
	wire	clk_i		,	clk_q		;
	wire	sign_hou	,	sign_qian	;
	wire   bothEdge;
 wire carryPulse,subtractionPulse;
	
	clk_div clk_div(
 clk_12,clk1,rst_n_i
    );
	
DPD	DPD(
	clk1,
	rst_n_i,
	
	data_i,//输入信号
	syn_o,//本地参考信号
	
	bothEdge,//携带输入信号边沿信息的脉冲序列
	sign_hou,//滞后信号
	sign_qian//超前信号
	
);

moniflop moniflop1
(
    clk1,      //32MHz系统时钟
    rst_n_i,      //高电平有效复位信号
    sign_qian,      
    carryPulse
);
moniflop moniflop2
(
    clk1,      //32MHz系统时钟
    rst_n_i,      //高电平有效复位信号
    sign_hou,      
    subtractionPulse
);
DCO DCO(
	clk1,
	rst_n_i,
	carryPulse,
	subtractionPulse,
	bothEdge,
	syn_o
);
	
//	assign	syn_o = clk_i	;
	
	
endmodule 
