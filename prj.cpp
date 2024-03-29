#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <dos.h>
#include <conio.h>
#include <io.h>
#include <string.h>
#include <time.h>
#define pi 3.1415926535
#define WIDTH  1110 //探测器的宽度
#define DIST  929 //旋转中心到探测器的距离
#define DIST0  9260  //焦点到旋转中心的距离
#define  frameN   360
#define imageX  200 //模型宽度
#define imageY  200 //模型长度
#define imageN  100  //模型长度
#define lmnta 0.5
#define Ire_Count 100

#define NumCount 10
#define  N0  frameN/4
int WDO[frameN];
void aculate_WDO();
float gg[frameN][WIDTH];  //投影数据
float pixel[imageX*imageY];   //重建图像的像素值
float pixel0[imageX*imageY];   //重建图像的初始像素值
float sintable[frameN];
float costable[frameN];
int sortdifferent( double X[],double Y[],double Z[],int Z0, int Z1);
void GetProjectMatrix(int length,int m, int n);//获得加权因子矩阵
void Load(char *filename); //读取投影数据
void save(int k,char *filename); //保存重建图像
void LoadPixel(char *filename); //读取重建图像初始值


double *X,*Y,*L;
int *J;
struct BIN_HEADER {	//********************* *.BIN file header struct
    char	s[492];		// Reserved
    float	min;		// Minimal value of data
    float	max;		// Maximal value of data
    int		width;		// Width of data
    int     height;		// Height of data
    int     depth;		// Depth of data (slices)
} ;
BIN_HEADER dataheader;
int main(int argc, char* argv[])
{
    int i,j;
    int m0,m,n;
    int ii,jj;
    dataheader.height = imageY;
    dataheader.width = imageX;
    for (i=0;i<imageX*imageY;i++)
    {
        pixel[i] = 0;
    }
    for(i = 0;i<frameN;i++)
    {
        sintable[i] = sin(i*pi*2/frameN);
        costable[i] = cos(i*2*pi/frameN);
    }
    char* file1 = "test-reset-576" ;
    Load(file1);  //读取投影数据
    //char* file00=  "SLJX_a0-360-lamt0.5-250" ;
    //LoadPixel(file00);   //读取图像初始值
    char *file2 = "result-image-701-3-";
    
    X = new double[2*imageN + 1];
    L = new double[2*imageN + 1];
    J = new int[2*imageN + 1];
    Y = new double[2*imageN+ 1];
    //aculate_WDO();
    int length;
    double time1;
    int Renum;
   	clock_t tick;
    
    //物体1
    for( Renum = 9;Renum<NumCount;Renum++)
    {
        for( m0 =0;m0<frameN;m0++)
        {
            for(n =0;n<WIDTH;n++)+
            {
                length = -4;
                GetProjectMatrix(length,m0,n);
            }
        }
        float xx0,yy0;
        for (i=0;i<imageY;i++)
            for(j=0;j<imageX;j++)
            {
                xx0 = j-imageX/2;
                yy0 = i-imageY/2;
                if(xx0*xx0+yy0*yy0>= imageX*imageY/4)
                    pixel[i*imageX+j] = 0;
                else if(pixel[i*imageX+j] < 1e-6)
                    pixel[i*imageX+j] = 0;
                
            }
        if((Renum+1)%10 == 0)
            save(Renum+1,file2);
    }
    
    tick = clock();
    time1 = (double)tick/CLK_TCK;
    printf("restruct time = %.5f",time1);
    delete []X;
    delete []Y;
    delete []J;
    delete []L;
    printf("OOOOOOOOKKKKKKKKK!");
    return 0;
}
//////////////////////////////////////////////////////////////////////////

int sortdifferent( double X[],double Y[],double Z[],int Z0, int Z1)
{
    int i,j,k;
    i =0;j=0;
    for( k =0;i<Z0 || j<Z1;k++)
    {
        if(i<Z0 && j< Z1)
        {
            if(X[i] < Y[j] )
            {
                Z[k] = X[i++];
            }
            else if( fabs(X[i] - Y[j])<1e-6)
            {
                Z[k] = X[i++];
                j++;
            }
            else
            {
                Z[k] = Y[j++];
            }
            
        }else
        {
            if(i<Z0) Z[k] = X[i++];
            if(j<Z1) Z[k] = Y[j++];
            
        }
    }
    return k;
}
//////////////////////////////////////////////////////////////////////////
void GetProjectMatrix(int length,int m, int n)
{
    int i,j,zd0,zd1,xd,yd,Z0,Z1;
    double k,b,xmin,xmax,ymin,ymax;
    double  x0,y0;  //焦点坐标
    double x1,y1;   //探元坐标
    x0 = -DIST*sintable[m]-length*costable[m]+imageX/2;  //焦点横坐标
    y0 = DIST*costable[m]-length*sintable[m]+imageY/2;   //焦点纵坐标
    x1 =DIST0*sintable[m]+(n-WIDTH/2)*costable[m]-length*costable[m]+imageX/2;  //探元横坐标
    y1 = -DIST0*costable[m]+(n-WIDTH/2)*sintable[m]-length*sintable[m]+imageY/2;   //探元纵坐标
    for( i = 0;i<=2*imageN;i++)
    {
        X[i] = 0;
        Y[i] = 0;
        L[i] = 0;
        J[i] = 0;
    }
    if(fabs(x1 - x0) <1e-6 )
    {
        if(x1>0 && x1<imageX)
        {
            xd = (int)(x1);
            Z0 = imageY;
            for( j =0;j<imageY;j++)
            { L[j] = 1;
                J[j] = xd + j*imageY;
            }
        }
        else
            Z0 =0;
    }
    else if(fabs(y1-y0)<1e-6)
    {
        if(y1>0 && y1<imageY)
        {
            yd = (int)(y1);
            Z0 = imageX;
            for( j =0;j<imageX;j++)
            {
                L[j] = 1;
                J[j] = yd*imageX + j;
            }
        }
        else
            Z0 = 0;
    }
    else
    {
        k = (y1-y0)/(x1 -x0);
        b = y0 - k*x0;
        if( k >0)
        {
            xmin = 0 > (-b/k) ? 0:(-b/k) ;
            xmax = imageX < ((imageY - b)/k)?imageX : ((imageY - b)/k);
            ymin = xmin*k + b;
            ymax = xmax*k + b;
        }
        if( k < 0)
        {
            xmin = 0 > ((imageY - b)/k) ? 0:((imageY - b)/k) ;
            xmax = imageX < (-b/k)?imageX : (-b/k);
            ymin = xmax*k + b;
            ymax = xmin*k + b;
            
        }
        if(xmin >= imageX || xmax <=0 )
        {
            Z0 =0;
        }
        else
        {
            for(i = ceil(xmin),j =0;i<=(xmax);i++)
                L[j++] = i;
            Z0 = j;
            //float k0 = 1/k;
            int dazhengshu = (ymax);
            int xiaozhengshu = ceil(ymin);
            if( k >0)
            {
                for(i = ceil(ymin),j = 0;i<=(ymax);i++) Y[j++] = (i-b)/k;
            }
            if( k < 0)
            {
                for(i = ((int)ymax),j = 0;i>=ceil(ymin);i--) Y[j++] = (i-b)/k;
            }
            
            Z1 = j;
            Z0 = sortdifferent(L,Y,X,Z0,Z1);
            for( i = 0;i<Z0;i++)
            {
                Y[i] = k*X[i] +b;
                if(Y[i] > imageX) Y[i] = imageX;
                if(Y[i] < 0)  Y[i] = 0;
            }
            float sum0 = 0;
            float sum1 = 0;
            float sum2 = 0;
            sum0 = sqrt((X[Z0-1]-X[0])*(X[Z0-1]-X[0])+(Y[Z0-1]-Y[0])*(Y[Z0-1]-Y[0]));
            
            for( i =0;i<Z0-1;i++)
            {
                J[i] = (int)((X[i+1]+X[i])/2) + ((int)((Y[i+1]+Y[i])/2))*imageN;
                //	L[i] = (X[i+1] -X[i])*sqrt(1+k*k);
                L[i]= sqrt((X[i+1] -X[i])*(X[i+1] -X[i])+(Y[i+1] -Y[i])*(Y[i+1] -Y[i]));
                
                sum1+=L[i];
            }
            //if(fabs(sum1-sum0)>1e-3) printf("求和计算错误：m=%d,n=%d,fabs(sum1-sum0)=%.6f-   %.6f=   %.6f\n",m,n,sum1,sum0,fabs(sum1-sum0));
            
        }
        
        
    }
    if(Z0>0)//计算加权因子
    {
        float tempt1 =0,tempt2=0,tempt3=0;
        
        //  传统迭代
        for( j =0;j<Z0-1;j++)
        {
            tempt1 += L[j]*pixel[J[j]];
            tempt2 += L[j]*L[j];
        }
        tempt3 = lmnta*(gg[m][n] - tempt1)/tempt2;
        for( j =0;j<Z0-1;j++)
            pixel[J[j]] = pixel[J[j]]+tempt3*L[j];
        //论文中的迭代算法
        //for( j =0;j<Z0-1;j++)
        //{ tempt1 += L[j]*pixel[J[j]];
        //tempt2 += L[j]*L[j];
        //}
        //
        //for( j =0;j<Z0-1;j++)
        //pixel[J[j]] = pixel[J[j]]+0.25*(gg[m][n] - tempt1)*L[j]/tempt2;
        //
    }
    
    // printf("总%d共次迭代，第  %d  次迭代,与射线相交的元素个数：  %d\n",m,n,Z0);
}
//////////////////////////////////////////////////////////////////////////
void aculate_WDO()
{
    
    float M[N0],N[N0],Dpq[N0][N0],up[N0],thitap[N0],wq[N0],aver_dp[N0],aver_up[N0],aver_thitap[N0],Dp[N0];
    float tempt0,tempt1,sum0,sum1;
    int i,j,k,p,q;
    for(i=0;i<N0-1;i++)
    {
        M[i] = (i+1)*360.0/frameN;   //保存未进行加权距离正交计算的角度
        N[i] = 0;     //保存进行加权距离正交计算后的角度
    }
    int L,Q;
    Q=1;
    for(L=N0-1;L>0;L--)
    {
        for(p=0;p<L;p++)
            for(q=0;q<Q;q++)
            {
                tempt0 = abs(M[p] - N[q]);
                Dpq[p][q] =  tempt0 < 90 - tempt0 ? tempt0 : 90-tempt0;
            }  //计算投影p、q之间的 距离
        for(q=0;q<Q;q++)
            wq[q] = (q+1)*1.0/Q ; //
        
        sum0 = 0;
        for(q=0;q<Q;q++)
            sum0 += wq[q]; //
        for(p=0;p<L;p++)
        {
            sum1 = 0;
            for(q=0;q<Q;q++)
                sum1 += wq[q]*(45-Dpq[p][q]) ;
            up[p] = sum1*1.0/sum0;
        }    //
        
        for(p=0;p<L;p++)
        {
            sum1 =0;
            for(q=0;q<Q;q++)
                sum1 += Dpq[p][q];
            aver_dp[p] = sum1/Q ;
        }//
        
        for(p=0;p<L;p++)
        {
            sum1 =0;
            for(q=0;q<Q;q++)
                sum1 += wq[q]*(Dpq[p][q] - aver_dp[p])*(Dpq[p][q] - aver_dp[p]);
            thitap[p] = sqrt(sum1/sum0);
        }
        //
        
        float min_up,max_up,min_thitap,max_thitap;
        min_up = up[0];
        max_up = up[0];
        min_thitap = thitap[0];
        max_thitap = thitap[0];
        for(p=1;p<L;p++)
        {
            if(min_up > up[p] ) min_up = up[p];
            if(max_up < up[p] ) max_up = up[p];
            if(min_thitap > thitap[p] ) min_thitap = thitap[p] ;
            if(max_thitap < thitap[p] ) max_thitap = thitap[p] ;
            
        }
        
        
        for(p=0;p<L;p++)
        {
            aver_up[p] = (up[p] - min_up)/(max_up -min_up);
            aver_thitap[p] = (thitap[p] - min_thitap)/(max_thitap - min_thitap);
            Dp[p] = aver_up[p]*aver_up[p] + 0.5*aver_thitap[p]*aver_thitap[p];
        }
        
        float min_D;
        min_D = Dp[0];
        k=0;
        for(p=1;p<L;p++)
        {
            if(min_D>Dp[p])
            {
                min_D = Dp[p];
                k=p;
            }
            
        }
        N[Q] = M[k];
        M[k] = M[L-1];
        
        Q++ ;
        
    }
    
    N[N0-1] = M[0];
    for(k=0;k<N0;k++)
    {
        WDO[k*4] = N[k]*frameN/360;
        WDO[k*4+1] =( N[k] +90)*frameN/360;
        WDO[k*4+2] =(N[k]+ 180)*frameN/360;
        WDO[k*4 + 3] = (N[k] + 270)*frameN/360;
    }
    
}
//////////////////////////////////////////////////////////////////////////
void Load(char* filename)
{
    FILE *fp ;
    fp = fopen(filename,"rb");
    int i,j;
    for( i =0;i<frameN;i++)
        for( j =0;j<WIDTH;j++)
            fread(&gg[i][j],4,1,fp);
    fclose(fp);
    printf("OOOOOOOOKKKKKKKKK!读取\n");
}
//////////////////////////////////////////////////////////////////////////
void LoadPixel(char *filename)
{
    FILE *fp ;
    char file[100];
    char *str0;
    str0 = ".bin";
    
    strcpy(file,filename);
    
    strcat(file,str0);
    fp = fopen(file,"rb");
    int i;
    for( i =0;i<imageX*imageY;i++)
        
        fread(&pixel[i],4,1,fp);
    fclose(fp);
    
}
//////////////////////////////////////////////////////////////////////////
void save(int k,char *filename)
{
    FILE *fp;
    char file[100];
    char str[5];
    char *str0;
    str0 = ".bin";
    sprintf(str,"%d",k);
    strcpy(file,filename);
    strcat(file,str);
    strcat(file,str0);
    
    fp = fopen(file,"wb");
    if(fp == NULL) 
    { printf("write_file Open error!");
        return;
    }
    int i,j;
    dataheader.depth = 1;
    dataheader.max = pixel[0];
    dataheader.min=pixel[0];
    for(i=0;i<imageY;i++)
        for(j=0;j<imageX;j++)
        {if(dataheader.min>pixel[i*imageX+j]) dataheader.min = pixel[i*imageX+j];
            if(dataheader.max<pixel[i*imageX+j]) dataheader.max = pixel[i*imageX+j];
            fwrite(&pixel[i*imageX+j],4,1,fp);
        }
    fwrite(&dataheader,sizeof(BIN_HEADER),1,fp);
    fclose(fp);
}
