#include <stdio.h>
#include <math.h>

#include "colorspace.h"

double fonction( double t ) ;
double fonction_inv( double t ) ;

void rgb_to_lab ( unsigned char color[3] ) {
  double m_11 = 0.4123908 , m_12 = 0.35758434 , m_13 = 0.18048079 ;
  double m_21 = 0.21263901 , m_22 = 0.71516868 , m_23 = 0.07219232 ;
  double m_31 = 0.01933082 , m_32 = 0.11919478 , m_33 = 0.95053215 ;
  double x , y , z , xn , yn , zn ;
  unsigned char r = color[0] ;
  unsigned char g = color [1] ;
  unsigned char b = color [2] ;
  x = m_11*r + m_12*g + m_13*b ;
  y = m_21*r + m_22*g + m_23*b ;
  z = m_31*r + m_32*g + m_33*b ;
  xn = m_11*255.0 + m_12*255.0 + m_13*255.0 ;
  yn = m_21*255.0 + m_22*255.0 + m_23*255.0 ;
  zn = m_31*255.0 + m_32*255.0 + m_33*255.0 ;
  color[0] = 116.0*fonction(y/yn)-16.0 ;
  color[1] = 500*(fonction(x/xn)-fonction(y/yn)) + 128 ; //ICI
  color[2] = 500*(fonction(y/yn)-fonction(z/zn)) + 128 ; //ICI
}

/*double binouse_rgb_to_lab (unsigned char color[3]) {

    double R = color[0]/255.0;
    double G = color[1]/255.0;
    double B = color[2]/255.0;

    // convert sRGB (R,G,B) to linear-rgb (r,g,b)
    r = (R <= 0.04045) ? R/12.92 : pow((R+0.055)/1.055,2.4);
    g = (G <= 0.04045) ? G/12.92 : pow((G+0.055)/1.055,2.4);
    b = (B <= 0.04045) ? B/12.92 : pow((B+0.055)/1.055,2.4);
 
    // convert to XYZ (assuming sRGB was D65)
    X =  r*0.4124564 + g*0.3575761 + b*0.1804375;
    Y =  r*0.2126729 + g*0.7151522 + b*0.0721750;
    Z =  r*0.0193339 + g*0.1191920 + b*0.9503041;
 
    // Rescale X/Y/Z relative to white point D65
    Xr = 0.95047, Yr = 1.0, Zr = 1.08883;
    xr = X/Xr;
    yr = Y/Yr;
    zr = Z/Zr;
 
    // tristimulus function
    eps = 216/24389.0, k = 24389/27.0;
    fx = (xr <= eps) ? (k * xr + 16.0)/ 116.0 : pow(xr, 1/3.0);
    fy = (yr <= eps) ? (k * yr + 16.0)/ 116.0 : pow(yr, 1/3.0);
    fz = (zr <= eps) ? (k * zr + 16.0)/ 116.0 : pow(zr, 1/3.0);
 
    // tranform to LAB  
    color[0] = ( 116 * fy ) - 16;
    color[0] = 500*(fx-fy);
    color[0] = 200*(fy-fz);
    
    }*/

void lab_to_rgb ( unsigned char color[3] ) {
    double n_11 = 3.24096994 , n_12 = -1.53738318 , n_13 = -0.49861076 ;
    double n_21 = -0.96924364 , n_22 = 1.8759675 , n_23 = -0.20397696 ;
    double n_31 = 0.05563008 , n_32 = -0.20397696 , n_33 = 1.05697151 ;
    double m_11 = 0.4123908 , m_12 = 0.35758434 , m_13 = 0.18048079 ;
  double m_21 = 0.21263901 , m_22 = 0.71516868 , m_23 = 0.07219232 ;
  double m_31 = 0.01933082 , m_32 = 0.11919478 , m_33 = 0.95053215 ;
    double x , y , z , xn , yn , zn ;
    unsigned char l = color[0] ;
    unsigned char a = color[1] - 128 ; //ICI
    unsigned char b = color[2] - 128 ; //ICI
    xn = m_11*255.0 + m_12*255.0 + m_13*255.0 ;
    yn = m_21*255.0 + m_22*255.0 + m_23*255.0 ;
    zn = m_31*255.0 + m_32*255.0 + m_33*255.0 ;
    x = xn*fonction_inv((l+16.0)/116.0 + a/500.0) ;
    y = yn*fonction_inv((l+16.0)/116.0) ;
    z = zn*fonction_inv((l+16.0)/116.0 - b/200.0) ;
    color[0] = n_11*x + n_12*y + n_13*z ;
    color[1] = n_21*x + n_22*y + n_23*z ;
    color[2] = n_31*x + n_32*y + n_33*z ;
}

void image_rgb_to_lab ( struct image *img ) {
    for ( int i = 0  ; i < img->height ; i++ ) {
	for ( int j = 0 ; j < img->width ; j++ ) {
	    rgb_to_lab ( &img->data[i][j*4] ) ;
	}
    }
}

void pal_image_lab_to_rgb ( struct pal_image *pali ) {
  printf("pal_image_lab_to_rgb\n");
    for ( int i = 0  ; i < pali->pal_len ; i++ ) {
      printf("lab = (%d,%d,%d)\n",pali->pal[i*3],pali->pal[i*3+1],pali->pal[i*3+2]); 
	lab_to_rgb ( &pali->pal[i*3] ) ;
	printf("rgb = (%d,%d,%d)\n",pali->pal[i*3],pali->pal[i*3+1],pali->pal[i*3+2]); 
    }
}

void pal_image_rgb_to_lab ( struct pal_image *pali ) {
  printf("pal_image_rgb_to_lab\n");
    for ( int i = 0  ; i < pali->pal_len ; i++ ) {
      printf("rgb = (%d,%d,%d)\n",pali->pal[i*3],pali->pal[i*3+1],pali->pal[i*3+2]); 
	rgb_to_lab ( &pali->pal[i*3] ) ;
	printf("lab = (%d,%d,%d)\n",pali->pal[i*3],pali->pal[i*3+1],pali->pal[i*3+2]); 
    }
}  

void rgb_to_luv (  unsigned char color[3] ) {
  double m_11 = 2.768892 , m_12 = 1.751748 , m_13 = 1.130160 ;
  double m_21 = 1.0 , m_22 = 4.5907 , m_23 = 0.0601 ;
  double m_31 = 0.0 , m_32 = 0.056508 , m_33 = 5.594292 ;
  double x , y , z , xn , yn , zn , up , upn , vp , vpn ;
  unsigned char r = color[0] ;
  unsigned char g = color [1] ;
  unsigned char b = color [2] ;
  x = m_11*r + m_12*g + m_13*b ;
  y = m_21*r + m_22*g + m_23*b ;
  z = m_31*r + m_32*g + m_33*b ;
  xn = m_11*255.0 + m_12*255.0 + m_13*255.0 ;
  yn = m_21*255.0 + m_22*255.0 + m_23*255.0 ;
  zn = m_31*255.0 + m_32*255.0 + m_33*255.0 ;
  up = 4.0*x/(x+15.0*y+3.0*z) ;
  vp = 9.0*y/(x+15.0*y+3.0*z) ;
  upn = 4.0*xn/(xn+15.0*yn+3.0*zn) ;
  vpn = 9.0*yn/(xn+15.0*yn+3.0*zn) ;
  color[0] = 116.0*fonction(y/yn)-16.0 ;
  color[1] = 13*color[0]*(up-upn) ;
  color[2] = 13*color[0]*(vp-vpn) ;
}

void image_rgb_to_luv ( struct image *img ) {
  for ( int i = 0  ; i < img->height ; i++ ) {
    for ( int j = 0 ; j < img->width ; j++ ) {
      rgb_to_luv ( &img->data[i][j*4] ) ;
    }
  }
}

double fonction( double t ){
    double t_lim = (6.0/29.0)*(6.0/29.0)*(6.0/29.0) ;
    if ( t > t_lim ) {
	return ( pow(t,1.0/3.0) ) ;
    } else {
	return ( (1.0/3.0)*(29.0/6.0)*(29.0/6.0)*t + (4.0/29.0) ) ;
    }
}

double fonction_inv( double t ) {
    double t_lim = (6.0/29.0)*(6.0/29.0)*(6.0/29.0) ;
    if ( t > t_lim ) {
	return ( t*t*t ) ;
    } else {
	return ( 3.0*(6.0/29.0)*(6.0/29.0)*(t - (4.0/29.0) )) ;
    }
}
