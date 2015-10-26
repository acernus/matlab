

load FBP_t1;
R0=400;%射线源到中心距离
R1=0;%503;探测器到中心距离
N=600;%探测器单元数目
M=360;%角度采用数目
delta_angle=2*pi/M;
g2=zeros(M,N,'single');%加权
for i=1:M
    for j=1:N
        u=j-N/2-0.5;
        g2(i,j)=t1(i,j)*R0/sqrt(R0^2+u^2);
    end
end

h=zeros(2*N-1,1,'single');%滤波函数
%R-L
for j=-(N-1):N-1
    if j==0
        h(j+N)=1/4;
    else if mod(j,2)==0
            h(j+N)=0;
        else h(j+N)=-1/(pi^2*j^2);
        end
    end
end
%S-L
% for j=-(N-1):N-1
%     h(j+N)=-2/(pi^2*(4*j^2-1));
% end
 gs=h./2;

g3r=zeros(M,N,'single');%滤波后投影
p1=zeros(N,1,'single');
for i=1:M
    for j=1:N
        p1(j,1)=g2(i,j);         
    end
    p2=conv(p1,gs,'same');
    for j=1:N
        g3r(i,j)=p2(j);
    end
end
imshow(g3r,[])
%%

PicN=512;%重建图像大小
delta=R0/(R0+R1);
sinangle=zeros(M,1,'single');
cosangle=zeros(M,1,'single');
for i=1:M
    angle=i*delta_angle;  
    sinangle(i)=sin(angle);
    cosangle(i)=cos(angle);
end

Pic1=zeros(PicN,PicN,'single');
delta_grid=1;%单像素宽度
for i=1:PicN
	y = (i- PicN/2 -0.5) * delta_grid;
	for j=1:PicN
		x = (j- PicN/2 -0.5) * delta_grid;  
        if x^2+y^2<(N/2)^2
           for m=1:M  
               U=R0-x*cosangle(m)-y*sinangle(m);                         
               ju=R0*(y*cosangle(m)-x*sinangle(m))/(R0-x*cosangle(m)-y*sinangle(m))+N/2+0.5;
                %L=sqrt((y*cosangle(m)-x*sinangle(m))^2+U^2);              
				ju0 = floor(ju);
                if ju>=1 && ju<=N-1
                   ju0=floor(ju);
                    Pic1(i,j)=Pic1(i,j)+(g3r(m,ju0)*(ju-ju0)+g3r(m,ju0+1)*(ju0+1-ju))*R0^2/U^2*delta_angle;
                else if ju==N
                     Pic1(i,j)=Pic1(i,j)+g3r(m,N)*R0^2/U^2*delta_angle;
                    end
                end                
           end
        end
    end
end
save FBP_Pic1 Pic1;
clear Pic1;
%%
PicN=512;
load FBP_Pic1;
ap=zeros(PicN,PicN,'single');
for i=1:PicN
    for j=1:PicN
        ap(i,j)=Pic1(PicN-i+1,j);
%         if ap(i,j)<0
%             ap(i,j)=0;
%         end
    end
end
clear Pic1;
figure;
imshow(ap,[0.05 1.75]);
%%