function  a = readdoublebin(file)
fid = fopen(file,'rb');
fseek(fid,-28,1);
 fmin = fread(fid,1,'double');
 fmax = fread(fid,1,'double');
 width = fread(fid,1,'long');
 height = fread(fid,1,'long');
 depth = fread(fid,1,'long');
fclose(fid);

fid = fopen(file,'rb');
a = fread(fid,[256 400],'double');%size [M，N]（读数据到M×N的矩阵中，数据按列存放） inf读整个文件
a = a';
fclose(fid);