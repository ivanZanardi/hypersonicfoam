//-----------------------------------------------------------------------------//
// * * * * * * COIL MAGNETIC FIELD CALCULATION (BIOT-SAVART LAW) * * * * * * * //
//-----------------------------------------------------------------------------//

// For formulations see 'University Physics with Modern Physics', pp. 932-934
// 13th edition by Hugh D. Young, Roger A. Freedman and A. Lewis Ford

#define _USE_MATH_DEFINES

#include <cmath>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

//-----------------------------------------------------------------------------//
//- INPUT parameters (for reentry capsules) -----------------------------------//
//-----------------------------------------------------------------------------//
// Coil center coordinates [m]:
   double x = 0.383191068;
   double y = 0.0;
   double z = 0.0; // NOTE: coil HAS TO lay in the yz plane
// Coil radius [m]:
   double R = 0.31989224615;
// Number of dl segments (see Biot-Savart law):
   int N = 1440; // 0.25 deg each segment
// Capsule nose x-coordinate [m]:
   double x_nose = 0.0;
// Desired B [T] at capsule nose:
   double B_nose = 0.5;

//-----------------------------------------------------------------------------//
//- Function for coil points calculation (dl segments ends) -------------------//
//-----------------------------------------------------------------------------//
void calculateCoilPoints(double **coil_points, const int steps, double x, double
			 y, double z, double R)
{
  double angle;
  const double pi = M_PI;
  int i;
  
  for (i = 0; i < steps; i++) 
  {
    angle = 2*pi*i/(steps-1);
    coil_points[i][0] = x;
    coil_points[i][1] = y + R*cos(angle);
    coil_points[i][2] = z + R*sin(angle);
  }
  return;
}

//-----------------------------------------------------------------------------//
//- Function to allocate bidimensional array ----------------------------------//
//-----------------------------------------------------------------------------//
void allocate2dArray(double **a, int n, int m)
{
  int i, j;
  a = (double**)malloc(n*sizeof(double*));
  for (i = 0; i < n; i++)
    a[i] = (double*)malloc(m*sizeof(double));

  for (i = 0; i < n; i++)
  {
    for (j = 0; j < m; j++)
      a[i][j] = 0.0;
  }
}

//-----------------------------------------------------------------------------//
//- Function to deallocate bidimensional array --------------------------------//
//-----------------------------------------------------------------------------//
void free2dArray(double **a, int n)
{
  int i;
  for(i = 0; i < n; i++)
  {
    free(a[i]);
  }
  free(a);
}

//-----------------------------------------------------------------------------//
//- Function for magnetic field calculation -----------------------------------//
//-----------------------------------------------------------------------------//
int main()
{
  FILE *ccx;
  FILE *ccy;
  FILE *ccz;
  int steps = N+1;

  // Files setup
  double** coil_points = NULL;
  double** cell_centers;
  FILE* B_file = fopen("0/B", "w");
  FILE* coil_file = fopen("coil", "w");
  FILE* centers_file = fopen("centers", "w");
  char buff[256]; // String dimension
  char buffx[256], buffy[256], buffz[256];
  int line_num_start = 21; // Starting line for extracting cell center values
  int num_cells;
  int counter;

  // Parameters for Biot-Savart law
  double pi = M_PI;
  double mu0 = 4*pi*pow(10, -7); // Vacuum magnetic permeability [N/A^2]
  double X = x-x_nose; // Distance 'capsule_nose-coil_center'
  double B0 = B_nose*pow(pow(X,2)+pow(R,2),1.5)/pow(R,3); // B at coil center
  double I = 2*R*B0/mu0; // Current intensity [A]
  double r[3], dl[3], r_unit[3]; // r = distance vector
                                 // dl = infinitesimal coil segment vector
                                 // r_unit = distance versor 
  double r_mag; // Distance between cell center and dl
  double c; // Constant term in Biot-Savart law
  double B[3]; // Magnetic field vector [T] (calculated for each cell)

  int i, j, k;

  //- Allocate memory for coil points -----------------------------------------//
  coil_points = (double**)malloc(steps*sizeof(double*));
  for (i = 0; i < steps; i++)
    coil_points[i] = (double*)malloc(3*sizeof(double));

  for (i = 0; i < steps; i++)
  {
    for (j = 0; j < 3; j++)
      coil_points[i][j] = 0.0;
  }
  //---------------------------------------------------------------------------//
  
  calculateCoilPoints(coil_points, steps, x, y, z, R);

  //- Read cell centers -------------------------------------------------------//
  ccx = fopen("0/Cx", "r");
  ccy = fopen("0/Cy", "r");
  ccz = fopen("0/Cz", "r");
  counter = 0;
  while (fgets(buff, sizeof(buff), ccx) != 0)
  {
    if (counter == line_num_start)
    {
      sscanf(buff, "%d", &num_cells); // Read and save cells number from Cx file
      printf("%d\n", num_cells);
      break;
    }
    counter++;
  }
  //---------------------------------------------------------------------------//

  //- Allocate memory for cell centers ----------------------------------------//
  cell_centers = (double**)malloc(num_cells*sizeof(double*));
  for (i = 0; i < num_cells; i++)
     cell_centers[i] = (double*)malloc(3*sizeof(double));

  for (i = 0; i < num_cells; i++)
  {
    for (j = 0; j < 3; j++)
      cell_centers[i][j] = 0.0; // Initialize cell centers array
  }
  //---------------------------------------------------------------------------//
 
  //- Reading cell center coordinates into memory -----------------------------//
  i = 0;
  while (fgets(buffx, sizeof(buff), ccx) != 0)
  {
    if (counter > line_num_start && counter < line_num_start+num_cells+1)
    {
      sscanf(buffx, "%lf", &cell_centers[i][0]);
      i++;
    }
    counter++;
  }

  i = 0;
  counter = 0;
  while (fgets(buffy, sizeof(buff), ccy) != 0)
  {
    if (counter > line_num_start+1 && counter < line_num_start+num_cells+2)
    {
      sscanf(buffy, "%lf", &cell_centers[i][1]);
      i++;
    }
    counter++;
  }
  
  i = 0;
  counter = 0;
  while (fgets(buffz, sizeof(buff), ccz) != 0)
  {
    if (counter > line_num_start+1 && counter < line_num_start+num_cells+2)
    {
      sscanf(buffz, "%lf", &cell_centers[i][2]);
      i++;
    }
    counter++;
  }
  //---------------------------------------------------------------------------//

  //= Starting file ===========================================================//

  fprintf(B_file,
"/*--------------------------------*- C++ -*----------------------------------*\\\n\
| =========                 |                                                 |\n\
| \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |\n\
|  \\\\    /   O peration     | Version:  v1706                                 |\n\
|   \\\\  /    A nd           | Web:      www.OpenFOAM.com                      |\n\
|    \\\\/     M anipulation  |                                                 |\n\
\\*---------------------------------------------------------------------------*/\n\
FoamFile\n\
{\n\
    version     2.0;\n\
    format      ascii;\n\
    class       volVectorField;\n\
    location    \"0\";\n\
    object      B;\n\
}\n\
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n\n\
dimensions\t[1 0 -2 0 0 -1 0];\n\n\
internalField   nonuniform List<vector>\n");

  fprintf(B_file, "%d\n(\n", num_cells); 

  //- Calculating B components ------------------------------------------------//
  for (k = 0; k < num_cells; k++)
  { 
    // Writing cell centers into file
    fprintf(centers_file, "%f %f %f;\n", cell_centers[k][0],
	    cell_centers[k][1], cell_centers[k][2]);
    // Initialize B
    B[0] = 0.0;
    B[1] = 0.0;
    B[2] = 0.0;
    for (i = 0; i < steps-1; i++) // Making the integral with a for-cycle
    {
      dl[0] = coil_points[i+1][0] - coil_points[i][0];
      dl[1] = coil_points[i+1][1] - coil_points[i][1];
      dl[2] = coil_points[i+1][2] - coil_points[i][2];
      r[0] = cell_centers[k][0] - coil_points[i][0];
      r[1] = cell_centers[k][1] - coil_points[i][1];
      r[2] = cell_centers[k][2] - coil_points[i][2];
      r_mag = sqrt(pow(r[0],2) + pow(r[1], 2) + pow(r[2], 2));
      r_unit[0] = r[0]/r_mag;
      r_unit[1] = r[1]/r_mag;
      r_unit[2] = r[2]/r_mag;
      c = mu0*I/(4*pi*pow(r_mag, 2));
      // Making the vectorial product "dl x r_unit":
      B[0] += c*(dl[1]*r_unit[2] - dl[2]*r_unit[1]);
      B[1] += c*(dl[2]*r_unit[0] - dl[0]*r_unit[2]);
      B[2] += c*(dl[0]*r_unit[1] - dl[1]*r_unit[0]); 
    }
    // Writing B field for each cell
    fprintf(B_file, "(%.8f %.8f %.8f)\n", B[0], B[1], B[2]);
  }
  // Writing B boundary conditions
  fprintf(B_file,
")\n;\n\n\
boundaryField\n\
{\n\
    inlet\n\
    {\n\ttype\t\tzeroGradient;\n    }\n\n\
    object\n\
    {\n\ttype\t\tzeroGradient;\n    }\n\n\
    #include \"include/boundaries\"\n\
}\n\n\
// * * * * * * * * * * * * * * * * * * *"
" * * * * * * * * * * * * * * * * * * //");

  //= File ended ==============================================================//

  fprintf(coil_file, "[");
  for (i = 0; i < steps; i++)
  {
    fprintf(coil_file, "%f %f %f;\n", coil_points[i][0], coil_points[i][1],
	    coil_points[i][2]);
  }
  fprintf(coil_file, "]");

  free2dArray(coil_points, steps);
  free2dArray(cell_centers, num_cells);

  fclose(B_file);
  fclose(coil_file);
  fclose(centers_file);
  fclose(ccx);
  fclose(ccy);
  fclose(ccz);

  return 0;  
}
