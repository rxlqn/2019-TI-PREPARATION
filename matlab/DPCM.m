% S_P_input = zeros(r,c);
% errorQ = zeros(r,c);

for i = 1:350
        if(i == 1)
            S_img_P = 128;
            R_img_P = 128;
        else    
            S_img_P = S_P_input(i - 1);
            R_img_P = S_P_input(i - 1);
        end
        error(i) = signal1(i) - S_img_P;
        errorQ(i) = Quant_Value(error(i)); 
        S_P_input(i) = S_img_P + errorQ(i);

        R_P_input(i) =errorQ(i)+R_img_P;
%       output(i,j) = Error_Code(errorQ,i,j,1);
end
plot(R_P_input);