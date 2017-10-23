#include "LabObject.h"

LabObject::~LabObject()
{
	/*delete point;
	delete color;
	delete bit;*/
}

LabObject::LabObject()
{
	//			register space
	//point = new Lab3D[get_Num_Of_Object()];
	//color = new myColor3[get_Num_Of_Object()];
	//bit = new int[get_Num_Of_Object()];
	//			initialize data
	for(int i = 0; i <= 100; i++)
	{
		for(int j = 0; j <= 200; j++)
		{
			for(int k = 0; k <= 200; k++)
			{
				//point[i][j][k].x = i;
				//point[i][j][k].y = j;
				//point[i][j][k].z = k;
				//cout<<point[index].x<<" "<<point[index].y<<" "<<point[index].z<<endl;
				color[i][j][k] = getColor1(i, j - 100, k - 100, bit[i][j][k]);
			}
		}
	}
}


//Lab3D LabObject::get_Posision(int index)
//{
//	Lab3D result;
//	result.x = index % (100 * 100);
//	result.y = index % (100)
//	return result;
//}

myColor3 LabObject::getColor1(int L, int a, int b, int &BIT)
{
	myColor3 result;
	BIT = 0;
	    //          from Lab to XYZ
    // actual CIE standard
    double T1 = 0.008856;
    // actual CIE standard
    double T2 = 903.3;
    double X, Y, Z;

    double F_y=(L+16)/116.0;
    double F_x = a/500.0 + F_y;
    double F_z = F_y - b / 200.0;

    double Xr = pow(F_x, 3);
    if(Xr > T1)
    {
        X = Xr;
    }
    else
    {
        X = (F_x * 116 - 16)/T2;
    }

    if(L > T2 * T1)
    {
        Y = pow(F_y, 3);
    }
    else
    {
        Y = L / T2;
    }

    double Zr = pow(F_z, 3);
    if(Zr > T1)
    {
        Z = Zr;
    }
    else
    {
        Z = (116 * F_z - 16)/T2;
    }

    // white point D65, (X = 95.04, Y = 100, Z = 108.88)
    X=X*0.950456;
    Y=Y*1.0;
    Z=Z*1.088754;

    //          from XYZ to RGB

    //          matrix M, from XYZ to sRGB, with D65 white correction
    double R= 3.2404542*X - 1.5371385*Y - 0.4985314*Z;
    double G=-0.9692660*X + 1.8760108*Y + 0.0415560*Z;
    double B= 0.0556434*X - 0.2040259*Y + 1.0572252*Z;

    if(R<=0.0031308)
    {
        R = 12.92 * R;
    }
    if(R > 0.0031308)
    {
        R = 1.055 * pow(R, 1 /2.4) - 0.055;
    }
    if(G<=0.0031308)
    {
        G= G * 12.92;
    }
    if(G > 0.0031308)
    {
        G= 1.055 * pow(G, 1 / 2.4) - 0.055;
    }
    if(B <= 0.0031308)
    {
        B= 12.92 * B;
    }
    if(B > 0.0031308)
    {
        B= 1.055 * pow(B, 1 / 2.4) - 0.055;
    }

    if(R > 1.0)
    {
		BIT = 1;
        R = 1.0;
    }
    if(R < 0.0)
    {
		BIT = 1;
        R = 0;
    }
    if(G > 1.0)
    {
		BIT = 1;
        G = 1.0;
    }
    if(G < 0.0)
    {
		BIT = 1;
        G = 0.0;
    }
    if(B > 1.0)
    {
		BIT = 1;
        B = 1.0;
    }
    if(B < 0.0)
    {
		BIT = 1;
        B = 0.0;
    }

	result.a=(int)(R*255 + 0.5);
	result.b=(int)(G*255 + 0.5);
	result.c=(int)(B*255 + 0.5);

    return result;
}
