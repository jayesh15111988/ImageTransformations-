#include <SImage.h>
#include <SImageIO.h>
#include <cmath>
#include <vector>

#define PI 3.14
#define E 2.718281828459045
#define True 1
#define False 0
using namespace std;
double ***matrix(int, SDoublePlane &,int, double);

// The simple image class is called SDoublePlane, with each pixel represented as
// a double (floating point) type. This means that an SDoublePlane can represent
// values outside the range 0-255, and thus can represent squared gradient magnitudes,
// harris corner scores, etc. 
//
// The SImageIO class supports reading and writing PNG files. It will read in
// a color PNG file, convert it to grayscale, and then return it to you in 
// an SDoublePlane. The values in this SDoublePlane will be in the range [0,255].
//
// To write out an image, call write_png_file(). It takes three separate planes,
// one for each primary color (red, green, blue). To write a grayscale image,
// just pass the same SDoublePlane for all 3 planes. In order to get sensible
// results, the values in the SDoublePlane should be in the range [0,255].
//

// Below are two helper functions that overlay rectangles / circles 
// on an image plane for visualization purpose. 

// Draws a rectangle on an image plane, using the specified gray level value and line width.
//
void overlay_rectangle(SDoublePlane &input, int _top, int _left, int _bottom, int _right, double graylevel, int width)
{
  for(int w=-width/2; w<=width/2; w++) {
    int top = _top+w, left = _left+w, right=_right+w, bottom=_bottom+w;

    // if any of the coordinates are out-of-bounds, truncate them 
    top = min( max( top, 0 ), input.rows()-1);
    bottom = min( max( bottom, 0 ), input.rows()-1);
    left = min( max( left, 0 ), input.cols()-1);
    right = min( max( right, 0 ), input.cols()-1);
      
    // draw top and bottom lines
    for(int j=left; j<=right; j++)
	  input[top][j] = input[bottom][j] = graylevel;
    // draw left and right lines
    for(int i=top; i<=bottom; i++)
	  input[i][left] = input[i][right] = graylevel;
  }
}

// Draws a circle on an image plane, given circle center coordinate and radius.
//
void overlay_circle(SDoublePlane &input, int x_center, int y_center, int radius, int color)
{
  int r2 = radius * radius;
  for (int x = -radius; x <= radius; x++) {
     double y = (double) (sqrt(r2 - x*x)+0.1);
     if(x_center+x >= 0 && x_center+x < input.cols()) {
       if(y_center+y >=0 && y_center+y < input.rows())
         input[y_center+y][x_center+x] = color;
       if(y_center-y >=0 && y_center-y < input.rows())
         input[y_center-y][x_center+x] = color;
     }
   }
}
SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
  SDoublePlane output(input.rows(), input.cols());

int a,b,c,d,i,j,k;
int **temp=0;
 temp=new int *[input.rows()];
 for (i=0;i<input.rows();i++)
 {
     temp[i]=new int [input.cols()];
     }     
int matrixcenterx = int(floor(input.cols()/2));
int matrixcentery = int(floor(input.rows()/2));
//Following convolution for 1-D matrices was referred from 2-D convolution from source 
//http://www.songho.ca/dsp/convolution/convolution.html#cpp_conv1d

for(i=0; i < input.rows(); i++)              
{
    for(j=0; j < input.cols(); j+=1)         
    {                
    output[i][j]=0;
    temp[i][j]=0;
        for(int m=0; m < col_filter.rows(); m++)     
        {
            int mm = col_filter.rows() - 1 - m;      
            
            
             int imagepointx = i + (m - (col_filter.rows()/2));

                if( imagepointx >= 0 && imagepointx < input.rows() && j >= 0 && j < input.cols()  )
                temp[i][j] += input[imagepointx][j] * col_filter[mm][0];
         
        }
        }
        }
        
        
        for(i=0; i < input.rows(); i++)              
{
    for(j=0; j < input.cols(); j++)         
    {                
    output[i][j]=0;
   
       
        for(int n=0; n < row_filter.cols(); n++) 
        {
               
             int nn = row_filter.cols() - 1 - n;  
int imagepointy = j + (n - (row_filter.cols()/2));
          
             
             if( imagepointy >= 0 && imagepointy < input.cols() && i >= 0 && i < input.rows()  )
             {
             //cout<<jj;
                output[i][j] += temp[i][imagepointy] * row_filter[0][nn];
}

             }
    }
}
/////////////$%$%$%$
  // Convolution code here
  for(int i=0;i<input.rows();i++)
{
free(temp[i]);

}
free(temp);

  return output;
}
// The rest of these functions are incomplete. These are just suggestions to 
// get you started -- feel free to add extra functions, change function
// parameters, etc.

// Given an image plane of size h x w, returns a subsampled image of size (h/k) x (w/k)
//
SDoublePlane gaussian_filter(const SDoublePlane &input, double sigma,int kersize)
{
SDoublePlane rowc(1,kersize);
SDoublePlane colc(kersize,1);

 SDoublePlane output(input.rows(), input.cols());
double **temp=0;
 temp=new double *[input.rows()];
 for (int i=0;i<input.rows();i++)
 {
     temp[i]=new double [input.cols()];
     }     

 
    double **mask=0;
 mask=new double *[kersize];
 for (int i=0;i<kersize;i++)
 {
     mask[i]=new double [kersize];
     }     

    int half=floor(kersize/2);
  double *temp1;
temp1=new double[kersize];
  double *temp2;
temp2=new double[kersize];
  double g=1/(2*PI*sigma*sigma);
  for(int i=0;i<kersize;i++)
    {
    int ii=i-half;
            temp1[i]=sqrt(g)*pow ( E,  ((-pow(-ii,2)) /(2*pow(sigma,2))));           
for(int j=0;j<kersize;j++)
            {
                   
                 int   jj=j-half;
                    temp2[j]=sqrt(g)*pow (       E,  ((-(pow(-jj,2)) /(2*pow(sigma,2))))         );
                    mask[i][j]=temp1[i]*temp2[j];

}
}

int mid = floor(kersize/2);
double *temp5;
temp5=new double[kersize];
for(int h=0;h<kersize;h++)
{
 if(mask[mid][0]==0)
 {                 temp5[h]=0;
                  }
 else
 {
 temp5[h]=(mask[mid][h]/mask[mid][0]);
}
}
//******************************************
int i=0;
for(int mm=0;mm<kersize;mm++)
{
if(i<kersize)
{
colc[i++][0]=mask[mm][0];
}
}

i=0;
  for(int nn=0;nn<kersize;nn++)
{
if(i<kersize)
{
rowc[0][i++]=temp5[nn];
}
}

 output = convolve_separable(input,rowc,colc);  
     

for(int i=0;i<input.rows();i++)
{
free(temp[i]);

}
free(temp);
for(int j=0;j<kersize;j++)
{
free(mask[j]);

}
free(mask);

free(temp1);
free(temp2);
free(temp5);



    return output;
}
SDoublePlane subsample(const SDoublePlane &input, const int k,int kersize,double sigma)
{
SImageIO::write_png_file("original.png", input, input, input);

//cout<<input.rows()<<"see this"<<input.cols();  
SDoublePlane output(ceil(input.rows()/k)+1, ceil(input.cols()/k)+1);

SDoublePlane input1=gaussian_filter(input,sigma,kersize);
//cout<<"see this row"<<input1.rows()<<endl;
//cout<<"see this column"<<input1.cols()<<endl;

SImageIO::write_png_file("gaussed.png", input, input, input);

    for (int x = 0; x < input.rows(); x+=k) 
{
//if(x>1935)
//{
//continue;
//}
//cout<<x<<endl;
    for (int y = 0; y < input.cols(); y+=k) 
{
//if(y>2591)
//{
//continue;
//}
//cout<<y<<endl;
        output[x/k][y/k] = input[x][y];
    }
}
  // Smooth then subsample the input image

  return output;
}

// Convolve an image with a separable convolution kernel
//
//################################################################
//################################################################

// Apply a Gaussian of the specified sigma to an image, and return the result
//


// Apply a mean filter to an image, returns the result
// 
SDoublePlane mean_filter(const SDoublePlane &input, int m, int n)
{
//m number of rows
//n number of columns
/*
SDoublePlane rows(1,kersize);
SDoublePlane cols(kersize,1);
for(int i=0;i<kersize;i++)
{
rows[0][i]=1/n;
cols[i][0]=1/m;
}
SDoublePlane output = convolve_separable(input,rows,cols);


*/

 SDoublePlane output(input.rows(), input.cols());
float matrixcenterx=input.cols()/2;
float matrixcentery=input.rows()/2;
int kersize=3;
              double **temp30=0;
    temp30=new double *[input.rows()];
 for (int var=0;var<input.rows();var++)
 {
     temp30[var]=new double [input.cols()];
     }
        double **temp31=0;
    temp31=new double *[input.rows()];
 for (int var=0;var<input.rows();var++)
 {
     temp31[var]=new double [input.cols()];
     }
 


              double **output1=0;
    output1=new double *[input.rows()];
 for (int var=0;var<input.rows();var++)
 {
     output1[var]=new double [input.cols()];
     }

 //###################################################
int q=0;
for(int e=0;e<input.rows();e++)
{
        for(int y=0;y<input.cols();y++)
{        
        temp30[e][y]=0;
        temp31[e][y]=0;
        }
        }
for(int i=0;i<input.rows();i++)
{
        
        for(int j=0;j<input.cols();j++)
        {
                output1[i][j]=0;
               // output[i][j]=0;
                }
                }

//int input[5][5]={{5,9,1,2,3},{1,2,3,6,9},{4,3,7,1,12},{3,5,2,12,56},{6,43,2,1,1}};
//int input[5][5]={{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}};
  
for(int i=0;i<input.rows();i++)
{
        q=0;
        for(int j=0;j<input.cols();j++)
        {
         //cout<<i<<"read na";
         //{ 
             if(q<input.cols())
             {      
                 if(q-1<0)
                 {
       
                temp30[i][q++]=input[i][j];
                
           
            }      
                else
                {
                    temp30[i][q]=input[i][j]+temp30[i][q-1];
                    q++;
                    }
                }
        } 
        }
       
        
            for(int i=0;i<input.rows();i++)
               {
        for(int w=1;w<input.cols();w++)
        {
                
                if(w-3<0)
                         {
               
                         output1[i][w-1]=temp30[i][w]/n;
                  

                         }
                else
                         {
                         
                         if(w==input.cols()-1)
                         {
                     
                        output1[i][w-1]=(temp30[i][w]-temp30[i][w-3])/n;
                         output1[i][w]=(temp30[i][w]-temp30[i][w-2])/n;
                   
                         }
                         else if(w>2 && w<input.cols()-1)
                         {
                    
                         output1[i][w-1]=(temp30[i][w]-temp30[i][w-3])/n;            

                         }
                         else
                         {
                             
                             }
                
                         }
                         }
                }  
          
        for(int i=0;i<input.cols();i++)
{
        q=0;
        for(int j=0;j<input.rows();j++)
        {
         //cout<<i<<"read na";
         //{ 
             if(q<input.rows())
             {      
                 if(q-1<0)
                 {
       
                temp31[q++][i]=output1[j][i];
                //cout<<"in first"<<temp31[q][i]<<endl;
           
            }      
                else
                {
                    temp31[q][i]=output1[j][i]+temp31[q-1][i];
                    q++;
                  //  cout<<"in second"<<temp31[q][i]<<endl;
                    }
                }
        } 
        }
       
        
                                   
               for(int i=0;i<input.cols();i++)
               {
        for(int w=1;w<input.rows();w++)
        {
                
                if(w-3<0)
                         {
               
                         output[w-1][i]=temp31[w][i]/m;
                  

                         }
                else
                         {
                         
                         if(w==input.rows()-1)
                         {
                     
                        output[w-1][i]=(temp31[w][i]-temp31[w-3][i])/m;
                         output[w][i]=(temp31[w][i]-temp31[w-2][i])/m;
                   
                         }
                         else if(w>2 && w<input.rows()-1)
                         {
                    
                         output[w-1][i]=(temp31[w][i]-temp31[w-3][i])/m;            

                         }
                         else
                         {
                             
                             }
                
                         }
                         }
                }  


//###################################################    
// Implement a m*n 2D mean filter with 1-d filters
for(int i=0;i<input.rows();i++)
{
free(output1[i]);

}
free(output1);

for(int i=0;i<input.rows();i++)
{
free(temp30[i]);

}
free(temp30);

for(int i=0;i<input.rows();i++)
{
free(temp31[i]);

}
free(temp31);

return output;
}

// Apply a sobel operator to an image, returns the result
// 
SDoublePlane sobel_gradient_filter(const SDoublePlane &input, bool _gx)
{
vector<int> sobelxcolumn (3);
vector<int> sobelxrow (3);

float matrixcenterx=input.rows()/2;
float matrixcentery=input.cols()/2;
int **temp = new int *[input.rows()];

 for (int i=0;i<input.rows();i++)
 {
     temp[i]=new int [input.cols()];
     }     
  SDoublePlane output(input.rows(), input.cols());
//vertical edge detection
if (_gx==False)
{
    sobelxcolumn.push_back(1);
    sobelxcolumn.push_back(2);
    sobelxcolumn.push_back(1);

    sobelxrow.push_back(-1);
    sobelxrow.push_back(0);
    sobelxrow.push_back(1);
    
}
else
{

    sobelxcolumn.push_back(-1);
    sobelxcolumn.push_back(0);
    sobelxcolumn.push_back(1);

    sobelxrow.push_back(1);
    sobelxrow.push_back(2);
    sobelxrow.push_back(1);
}
//discard next two functions
for(int f=0; f < input.rows(); f++)              
{
    for(int j=0; j < input.cols(); j++)         
    {                
//    out[i][j]=0;
    temp[f][j]=0;
        for(int m=0; m < 3; m++)     
        {
            int mm = 3 - 1 - m;      
            
            
             int imagepointx = f + (m - 1);
                if( imagepointx >= 0 && imagepointx < input.rows() && j >= 0 && j < input.cols()  )
                temp[f][j] += input[imagepointx][j] * sobelxcolumn[mm];
         
        }
        }
        }
        
        
        for(int q=0; q < input.rows(); q++)              
{
    for(int w=0; w < input.cols(); w+=1)         
    {                
    output[q][w]=0;
   
       
        for(int n=0; n < 3; n++) 
        {
               
             int nn = 3 - 1 - n;  
int imagepointy = w + (n - 1);
          
             
             if( imagepointy >= 0 && imagepointy < input.cols() && q >= 0 && q < input.rows()  )
             {
             //cout<<jj;
                output[q][w] += temp[q][imagepointy] * sobelxrow[nn];
}

             }
    }
}
//Remove the comment
//SDoublePlane output = gaussian_filter(input,sobelxrow,sobelxcolumn);
  // Implement a sobel gradient estimation filter with 1-d filters
  
for(int i=0;i<input.rows();i++)
{
free(temp[i]);

}
free(temp);

  return output;

}

// Apply an edge detector to an image, returns the binary edge map
// 
SDoublePlane find_edges(const SDoublePlane &input, double thresh)
{
SDoublePlane output(input.rows(), input.cols());
SDoublePlane outputx = sobel_gradient_filter(input,True);
SDoublePlane outputy = sobel_gradient_filter(input,false);

for(int i=0;i<outputx.rows();i++)
{
        for(int j=0;j<outputy.cols();j++)
        {
               output[i][j]=sqrt(pow(outputx[i][j],2)+pow(outputy[i][j],2)) ;


//if(i<outputx.rows()-1 && i>0 && j<outputy.cols()-1 && j>0 )
//{
//output[i+1][j+1]=sqrt(pow(outputx[i+1][j+1],2)+pow(outputy[i+1][j+1],2)) ;
//output[i-1][j-1]=sqrt(pow(outputx[i-1][j-1],2)+pow(outputy[i-1][j-1],2)) ;
//output[i][j-1]=sqrt(pow(outputx[i][j-1],2)+pow(outputy[i][j-1],2)) ;
//output[i-1][j]=sqrt(pow(outputx[i-1][j],2)+pow(outputy[i-1][j],2)) ;
//}

//cout<<output[i][j]<<endl;               
//change as implementation goes on
if (output[i][j]>=thresh)
 {

output[i][j]=0;

}


                                        else
                                        {
                                            output[i][j]=255;
                                            }
               
        }
}


for(int i=0;i<outputx.rows();i++)
{
        for(int j=0;j<outputy.cols();j++)
        {
if(i<outputx.rows()-1 && i>0 && j<outputy.cols()-1 && j>0 )
{
if((output[i][j]>output[i+1][j+1]) && (output[i][j]>=output[i-1][j-1]))
{

output[i][j]=0;
}

else
{
output[i][j]=255;
}

} 
}

}
        
  // Implement an edge detector of your choice,
  // use your sobel gradient operator to compute the gradient magnitude
  
  return output;
}

// Apply a circle detector to an image, returns the detection result
// 
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
double ***matrix(const int range1,const int range2,SDoublePlane &input,int radiushough,double wtthresh)
{
//cout<<"in **matrix"<<endl;
//int range1=31;
//int range2=10;
//cout<<"paav";
int a,b;
 double ***temp=0;

    temp = new double**[input.rows()];
  for (int i = 0; i < input.rows(); ++i) 
{
    temp[i] = new double*[input.cols()];

    for (int j = 0; j < input.cols(); ++j)
      temp[i][j] = new double[range1];
  }

for(int i=0;i<input.rows();i++)
{
for(int j=0;j<input.cols();j++)
{
for(int k=0;k<range1;k++)
{
temp[i][j][k]=0;
}

}
}

int rr;
for(int i=0;i<input.rows();i++)
{

for(int j=0;j<input.cols();j++)
{
if(input[i][j]==0)
{
for(int r=0;r<range1;r++)
{
if(r+range2==radiushough)
{
for(double theta=0;theta<=2*PI;theta+=0.00174)
{
a=(round(i-((r+range2)*cos(theta))));
b=(round(j-((r+range2)*sin(theta))));
if(a<input.rows() &&a>=0 && b>=0 && b<input.cols())
{
temp[a][b][r]++;
//cout<<temp[a][b][r]<<endl;

if(temp[a][b][r]>wtthresh)
{
cout<<"how waah"<<temp[a][b][r]<<"how how "<<"a--------> "<<a<<" b------->"<<b<<" r-------> "<<r+10<<endl;
}
}
}
}
}
}
}
}
/////////////////////
for(int i=0;i<input.rows();i++)
{

for(int j=0;j<input.cols();j++)
{
for(int r=0;r<range1;r++)
{
if(j<input.cols()-1 && i<input.rows()-1)
{
//if(r>2)
//{
if((abs(temp[i][j][r]-temp[i][j+1][r])<300) || (abs(temp[i][j][r]-temp[i+1][j][r])<300)||(abs(temp[i][j][r]-temp[i+1][j-1][r])<300)||(abs(temp[i][j][r]-temp[i][j+2][r])<300)||(abs(temp[i][j][r]-temp[i+2][j][r])<300)||(abs(temp[i][j][r]-temp[i+2][j][r-2])<300)||(abs(temp[i][j][r]-temp[i+1][j+1][r-1])<300))
{
//cout<<temp[i][j][r]<<endl;
//cout<<temp[i][j+1][r]<<endl;
//cout<<temp[i+1][j][r]<<endl;
//cout<<i<<" "<<i+1<<endl;
//cout<<j<<" "<<j+1<<endl;
//cout<<endl<<endl;
//cout<<temp[i][j][r]-temp[i][j+1][r]<<endl;
//cout<<temp[i][j][r]-temp[i+1][j][r]<<endl;
//cout<<endl;
temp[i][j][r]=0;
//cout<<temp[i][j][r]<<endl;
//cout<<temp[i][j+1][r]<<endl;
//cout<<temp[i+1][j][r]<<endl;

}
}
}
}
//}
}

/*for(int i=0;i<input.rows();i++)
{

for(int j=0;j<input.cols();j++)
{
for(int r=0;r<range1;r++)
{
cout<<temp[i][j][r]<<"how how "<<"i--------> "<<i<<" j------->"<<j<<" r-------> "<<r+10<<endl;
}
}
}*/

/////////////////////
return temp;
}
//////////////////////////////////
SDoublePlane hough(SDoublePlane &input,int radiushough,double wtthresh)
{
SDoublePlane output(input.rows(),input.cols());
int x,y;
 // Hough transform for circle detection
  // This skeleton code places a fixed cirlce at the image center. 
//  SDoublePlane input1 = input;
//cout<<input1.rows()/2<<endl<<input1.rows()/2;
 //overlay_circle(input, input.cols()/2,input.rows()/2, 10, 255);
//overlay_circle(input, input.cols()/2,input.rows()/4, 10, 255);
//overlay_circle(input, 35,25, 20, 255);
//overlay_circle(input, 90,25, 10, 255);
//overlay_circle(input, 25,25, 12, 255);

//SImageIO::write_png_file("circle_hough14.png",input,input,input);
int range1=40;
int range2=10;
//int range3=80;
int *row=0;
int *column=0;
int *radius=0;
int r=0,rowmat=0,colmat=0;

radius = new int[range1*input.rows()*input.cols()];
row=new int[range1*input.rows()*input.cols()];
column=new int[range1*input.rows()*input.cols()];
for(int i=0;i<input.rows()*input.cols()*range1;i++)
{
row[i]=0;
column[i]=0;
radius[i]=0;
}
//SDoublePlane output(input1.rows(),input1.cols());
int a,b;
double ***temp=0;
int range=10;
temp=matrix(range1,range2,input,radiushough,wtthresh);
//cout<<"just caled";
int var=0;
for(int i=0;i<input.rows();i++)
{
for(int j=0;j<input.cols();j++)
{
for(int k=0;k<range1;k++)
{
//cout<<i<<endl;
//cout<<temp[i][j][k]<<endl;
if(temp[i][j][k]>wtthresh)
{
//cout<<"caught";
//cout<<"fffought"<<k<<endl;
row[rowmat++]=i;
column[colmat++]=j;
radius[r++]=k;
//cout<<"atleast";
//rowmat++;
//colmat++;
//r++;
//var++;
cout<<"Value **-->"<<temp[i][j][k]<<"this is i-->  "<<i<<"this is j --> "<<j<<"this is radiius --->  "<<k+10<<endl;
}
}
}

}
/*for(int i=0;i<10;i++)
{
//cout<<row[i]<<" "<<column[i]<<" "<<radius[i]<<" "<<endl<<"end?";
}*/
int val=0;
for(int i=0;i<input.rows()*input.cols()*range1;i++)
{
//cout<<"0-------->"<<i<<endl;
//cout<<row[i]<<"bagh bagh"<<endl;
if(row[i]==0)
{
val=i;
cout<<val;
cout<<"broke";
break;
//cout<<"broke";
}


for(double theta=0;theta<=2*PI;theta+=0.00174)
{
//cout<<theta<<"<<<<--- theta";
//cout<<cos(theta)<<"theta";
//cout<<row[i]+(radius[i]+range2)*cos(theta)<<"see that"<<endl;
x=(round(row[i]+(radius[i]+range2)*cos(theta)));
y=(round(column[i]+(radius[i]+range2)*sin(theta)));
//cout<<radius[i]+range2<<"that is radius"<<endl;
//cout<<x<<endl<<y<<endl;
if (x<input.rows() && y<input.cols()&& x>=0 && y>=0)
{
//cout<<"-------x>"<<x<<"------y>"<<y<<endl;
//cout<<"paagal"<<x<<endl;
// 111 cols
// 71 rows
//cout<<input.rows()<<"-------<<<<<<rows"<<endl;
//cout<<input.cols()<<"<<<<<<<<---cols"<<endl;
output[x][y]=255;
//cout<<i<<" "<<endl;
//cout<<"Value of -->"<<row[i]<<"this is i--<<<<<<  "<<column[i]<<"this is j --<<<<<< "<<radius[i]<<"this is radiius ---<<<<<  "<<endl;

}
}
}





//cout<<var<<endl;
for(int i=0;i<input.rows();i++)
{
for(int j=0;j<input.cols();j++)
{
delete [] temp[i][j];
}
delete [] temp[i];
}
delete temp;

  
//SDoublePlane overlay_plane = input;
  //overlay_circle(overlay_plane, input.cols()/2, input.rows()/2, 10, 255);

  //return overlay_plane;
return output;
}



//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^




int main(int argc, char *argv[])
{
//cout<<"please enter the value";
//sigma-------->    threshold ----------> kernel size
float sigma,threshold,wtthresh;
int kersize,radiushough;
//cout<<argc<<"------<<<<<<"<<endl;
  if((argc < 2))
    {
      cerr << "usage: " << argv[0] << " input_image" << endl;
      return 1;
    }
if(argc>=3)
{
//sigma=double(argv[2]);
sigma=atof(argv[2]);
cout<<"sigma argv 2"<<sigma<<endl;
}
else
{
sigma=4.5;
}
if(argc>=4)
{

threshold=atof(argv[3]);
cout<<"threshold argv 3"<<threshold<<endl;

}
else
{

threshold=2;

}
if(argc>=5)
{

kersize=atoi(argv[4]);
cout<<"kernel argv 4"<<kersize<<endl;

}
else
{
kersize=3;
}

if(argc>=6)
{

radiushough=atoi(argv[5]);
cout<<"radius argv 5"<<radiushough<<endl;

}
else
{

radiushough=0;

}
if(argc>=7)
{

wtthresh=atof(argv[6]);
cout<<"threshold argv 6"<<wtthresh<<endl;

}
else
{

wtthresh=900;

}
  string input_filename(argv[1]);
  SDoublePlane input_image= SImageIO::read_png_file(input_filename.c_str());
SImageIO::write_png_file("original.png", input_image, input_image, input_image);
SDoublePlane gaussian = gaussian_filter(input_image,sigma,kersize);
  SImageIO::write_png_file("gaussian26.png", gaussian, gaussian, gaussian);
SDoublePlane subsampled = subsample(gaussian,4,kersize,sigma);
SImageIO::write_png_file("subsampled25.png", subsampled, subsampled, subsampled);  
SDoublePlane edges = find_edges(subsampled,threshold);
SImageIO::write_png_file("edges25.png", edges, edges, edges);
//cout<<radiushough<<"read this "<<wtthresh;
SDoublePlane houghc = hough(edges,radiushough,wtthresh);
SImageIO::write_png_file("hough25.png", houghc, houghc, houghc);

  // subsample the input image
// SDoublePlane subsampled = subsample(input_image,3,kersize,sigma);
  //SImageIO::write_png_file("subsampled.png", subsampled, subsampled, subsampled);
//SDoublePlane mean = mean_filter(input_image,10,10);
  //SImageIO::write_png_file("mean.png", mean, mean, mean);
 //SDoublePlane sobel = sobel_gradient_filter(input_image,True);
  //SImageIO::write_png_file("sobely.png", sobel, sobel, sobel);
//SDoublePlane houghc = hough(input_image);
//  SImageIO::write_png_file("hough.png", houghc, houghc, houghc);

  // compute gradient magnitude map of the input image
  SDoublePlane gradient_x = sobel_gradient_filter(input_image, True);
  SDoublePlane gradient_y = sobel_gradient_filter(input_image, False);
  SDoublePlane gradmag(input_image.rows(), input_image.cols());
  SImageIO::write_png_file("gradmag.png", gradmag, gradmag, gradmag);
  
  // find edges in the input image
  //SDoublePlane edges = find_edges(gaussian,8);
  //SImageIO::write_png_file("edges.png", edges, edges, edges);
//  SDoublePlane gaussian1 = gaussian_filter(input_image,1.9);
//cout<<gaussian1.rows()<<endl<<gaussian1.cols()<<endl;  
//SImageIO::write_png_file("gaussian1.png", gaussian1, gaussian1, gaussian1);

  // detect circles in the input image, and visualize the result
//SDoublePlane overlay_plane = hough_circles(input_image, input_image.cols()/2, input_image.rows()/2, 10);
  //SImageIO::write_png_file("detected_circle23.png",overlay_plane,overlay_plane,overlay_plane);
}
