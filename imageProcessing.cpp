
//============================================================================
// Name        : Image Processing using C++
// Author      : Piyush Khopkar
// Description : Basic image processing operations in an input .PGM image
//              - Thresholding- Binary and partial
//              - Making an identical copy of the input image
//              - Transpose of the input image
//              - Unsharp masking
//              - Mirroring the input image

//              - Perfrom operator overloading of '+' operator for
//                  - Adding two images(Image1 + Image2)
//                  - Adding a constant to an image (Image1 + constant)
//                  - Adding an image to a constant (constant + Image1)
//============================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//using namespace std;                      // gives compiler warning

/* START Type, Class, and Function Definitions */

/* Structure */
typedef struct image_info_
  {
      int image_width;
      int image_height;
  } image_info_;

/* Image class */
class Image
  {
    private:
      FILE *input_file;
      FILE *output_file;
      int *blurred_pixels;
      void open_file(int, int, char *);
      void read_pixel_values();
      void gaussian_blur();
      void binary_threshold(int);
      void write_pixel(int, FILE *);
      image_info_ image_info;
    protected:
      int *original_pixel_values;
      int *binary_threshold_pixels;
    public:
      void get_header();
      int get_threshold();
      void threshold_image(int, char);
      void transpose_image();
      void mirror_image();
      void unsharp_mask();
      
      /* operator overloading*/
      void operator+(Image image2);                             // image + image
      void operator+(int);                                      // constant + image
      friend void operator+(int, Image&);                       // image + constant

      /* Constructor*/
      Image();
      Image(int);
      Image(char *, char *);
      
      /* Destructor*/
      ~Image();
  };

/* Define the friend function */
void operator+(int, Image&);                                    // image + constant

char *get_file_name(int, int, char *);
char menu();

/* START main() and utility functions */
int main(int argc, char **argv)
  {
    bool countnue_program = true;
    while (countnue_program)                                    // Continue program until user chooses to exit
      {
        char option = menu();                                   // Get option form user

        if (option == 'E')                                      // Exit program
          return 0;

        // Image user_image;                                    // Use empty constructor
          
        /* Pass 1 for input file, 2 for output file  */
        Image user_image(get_file_name(1, argc, argv[1]), get_file_name(2, argc, argv[2]));
        
        user_image.get_header();                                // Get header information and copy to new file

        if (option == 'U' || option == 'L' || option == 'F')    // If thresholding, get threshold value and threshold image
          {
            int threshold;
            threshold = user_image.get_threshold();
            user_image.threshold_image(threshold, option);      // Copy image making appropriate change based on option and threshold
          }
        else if (option == 'C') // Copy image
          user_image.threshold_image(0, option);                // Copy image making appropriate change based on option and threshold
        else if (option == 'T')                                 // Transpose image
          user_image.transpose_image();
        else if (option == 'M')                                 // Mirror image
          user_image.mirror_image();
        else if (option == 'N')                                 // Unsharp mask
          user_image.unsharp_mask();
        else if (option == '1')
          {
            Image image2(0);
            user_image + image2;
          }
        else if (option == '2')
          {
            int constant;
            printf("\nEnter your constant: ");
            scanf(" %d", &constant);
            user_image + constant;
          }
        else if (option == '3')
          {
            int constant;
            printf("\nEnter your constant: ");
            scanf(" %d", &constant);
            constant + user_image;
          }
      }

    printf("\n");
    return 0;
  }

char *get_file_name(int option, int argc, char *file_name_passed)
  {
    char *file_name;
    if (argc < 3)                                                   // If input and output filenames are not specified, ask for them
      {
        file_name = (char *) malloc(sizeof(char) * 25);
        if (option == 1)
          printf("\nEnter the name of your input file: ");
        else
          printf("Enter the name of your output file: ");
        scanf("%s", file_name);
      }      
    else                                                            // If filenames are specfied, get them
      file_name = file_name_passed;
    return file_name;
  }

// Get image processing option form user
char menu()
  {
    char option;
    printf("\nMenu"
           "\n  U - upper threshold"
           "\n  L - lower threshold"
           "\n  F - full threshold"
           "\n  C - copy the image"
           "\n  T - transpose image"
           "\n  M - mirror (flip horizontally) image"
           "\n  N - unsharp mask"
           "\n  1 - add binary thresholded image to original"
           "\n  2 - add constant to image"
           "\n  3 - add image to constant"
           "\n  E - exit program"
           "\n  Enter choice: ");
    scanf(" %c", &option);

    while (option != 'U' && option != 'L' && option != 'F' && option != 'C' && option != 'T' && option != 'M' && option != 'N' && option != 'E' && option != '1' && option != '2' && option != '3') // Make sure option entered is valid
      {
        printf("\nError.  Input invalid.  Try again.\n");
        printf("Menu"
           "\n  U - upper threshold"
           "\n  L - lower threshold"
           "\n  F - full threshold"
           "\n  C - copy the image"
           "\n  T - transpose image"
           "\n  M - mirror (flip horizontally) image"
           "\n  N - unsharp mask"
           "\n  1 - add binary thresholded image to original"
           "\n  2 - add constant to image"
           "\n  3 - add image to constant"
           "\n  E - exit program"
           "\n  Enter choice: ");
        scanf(" %c", &option);
      }

    return option;
  }

// ----------------------- END main() and utility functions -----------------------

// ----------------------- START Class Constructors and Deconstructors -----------------------

/* For an emapty constructor, take the default input and output image */
Image::Image()
  { 
    input_file = NULL;
    output_file = NULL;
    original_pixel_values = NULL;
    blurred_pixels = NULL;
    binary_threshold_pixels = NULL;

    /* Open input file */
    input_file = fopen("muimg.pgm", "r");
    if (!input_file)
      printf("\nError.  The default image file (muimg.pgm) could not be found.\n\n");

    /* Open output file */
    char *output_file_name = (char *) malloc(sizeof(char) * 25);
    printf("\nEnter the name of your input file: ");
    scanf("%s", output_file_name);
    while (strstr(output_file_name, ".pgm") == NULL)                                // Make sure output file has correct extension
      {
        printf("\nError.  Your output file must have a '.pgm' extension. Try again.\n"
                 "Enter the name of your output file: ");
        scanf("%s", output_file_name);
      }
    output_file = fopen(output_file_name, "w+");
    free(output_file_name);
  }

Image::Image(int x)
  {
    input_file = NULL;
    output_file = NULL;
    original_pixel_values = NULL;
    blurred_pixels = NULL;
    binary_threshold_pixels = NULL;
  }

/* Take both input and output image from the user */
Image::Image(char *input_file_name, char *output_file_name)
  {
    input_file = NULL;
    output_file = NULL;
    original_pixel_values = NULL;
    blurred_pixels = NULL;
    binary_threshold_pixels = NULL;

    input_file = fopen(input_file_name, "r");
    while (input_file == NULL || strstr(input_file_name, ".pgm") == NULL) // Make sure input file has correct extension and exists
      {
        if (input_file == NULL && strstr(input_file_name, ".pgm") == NULL)
          printf("\nError.  Your input file was not found and must have a '.pgm' extension. Try again.\n"
                   "Enter the name of your input file: ");
        else if (input_file == NULL )
          printf("\nError.  Your input file was not found. Try again.\n"
                   "Enter the name of your input file: ");
        else // If file doesn't have right extension
          {
            printf("\nError.  Your input file must have a '.pgm' extension. Try again.\n"
                   "Enter the name of your input file: ");
            fclose(input_file);
          }
        scanf("%s", input_file_name);
        input_file = fopen(input_file_name, "r");
      }

    while (strstr(output_file_name, ".pgm") == NULL)  // Make sure output file has correct extension
      {
        printf("\nError.  Your output file must have a '.pgm' extension. Try again.\n"
                 "Enter the name of your output file: ");
        scanf("%s", output_file_name);
      }
    output_file = fopen(output_file_name, "w+");
  }

Image::~Image()
  {
    if (input_file)
      fclose(this->input_file);                                             // Close input file
    if (output_file)
      fclose(this->output_file);                                            // Close output file

    // if (original_pixel_values)
      // delete[] original_pixel_values;
    // if (blurred_pixels)
    //   delete[] blurred_pixels;
    // if (binary_threshold_pixels)
      // delete[] binary_threshold_pixels;
  }

// ----------------------- END Class Constructors and Deconstructors -----------------------

// ----------------------- START Class Member Functions -----------------------

// Copy header to new file and set file pointer
void Image::get_header()
  {
    int new_line_count = 0;
    char copy_char;
    int characters_before_width = 0;
    while (new_line_count < 4) // Copy 4 lines for header
      {
        fscanf(input_file, "%c", &copy_char);
        fprintf(output_file, "%c", copy_char);
        if (copy_char == '\n')                                              // Count number of new line characters
          new_line_count++;
        if (new_line_count < 2)                                             // Count number of characters up to width value
          characters_before_width++;
      }

    fseek (input_file, characters_before_width+1, SEEK_SET);                // Set file pointer to location of image width value
    fscanf(input_file, "%d", &image_info.image_width);                      // Get image width
    fscanf(input_file, "%d", &image_info.image_height);                     // Get image height

    int trash_value;
    fscanf(input_file, "%d", &trash_value);                                 // Move file pointer past max gray value

    return;
  }

// Get threshold value
int Image::get_threshold()
  {
    int threshold;
    printf("\nEnter a threshold value (must be between 0 and 255 inclusive): ");
    scanf("%d", &threshold);

    while (threshold < 0 || threshold >= 255)                               // Make sure threshold value is in valid range
      {
        printf("\nError.  Your threshold value must be between 0 and 254 inclusive (must be less than max gray value). Try again.\n"
               "Enter a threshold value: ");
        scanf("%d", &threshold);
      }

    return threshold;
  }

// Threshold or copy image based on options
void Image::threshold_image(int threshold, char option)
  {
    int pixel_value;
    if (option == 'U')                                                      // Upper threshold
      while (!feof(input_file))
        {
          fscanf(input_file, "%d", &pixel_value);
          if (pixel_value <= threshold)
            fprintf(output_file, "%d\n", 0);
          else
            fprintf(output_file, "%d\n", pixel_value);
        }
    else if (option == 'L')                                                 // Lower threshold
      while (!feof(input_file))
        {
          fscanf(input_file, "%d", &pixel_value);
          if (pixel_value <= threshold)
            fprintf(output_file, "%d\n", pixel_value);
          else
            fprintf(output_file, "%d\n", 255);
        }
    else if (option == 'F')                                                 // Full threshold
      while (!feof(input_file))                                             // Full threshold
        {
          fscanf(input_file, "%d", &pixel_value);
          if (pixel_value <= threshold)
            fprintf(output_file, "%d\n", 0);
          else
            fprintf(output_file, "%d\n", 255);
        }
    else                                                                    // Copy image
      while (!feof(input_file)) // Copy image
        {
          fscanf(input_file, "%d", &pixel_value);
          fprintf(output_file, "%d\n", pixel_value);
        }

    return;
  }

void Image::binary_threshold(int threshold)
  {
    binary_threshold_pixels = (int *) (malloc(sizeof(int) * image_info.image_width * image_info.image_height));
    for (int i = 0; i < image_info.image_height * image_info.image_width; i++)
      {
        if (original_pixel_values[i] <= threshold)
          binary_threshold_pixels[i] = 0;
        else
          binary_threshold_pixels[i] = 255;
        // int a = 1;
      }
  }

// Read original pixel values into array from file
void Image::read_pixel_values()
  {
    original_pixel_values = (int *) (malloc(sizeof(int) * image_info.image_width * image_info.image_height));
    int i;
    for (i = 0; i < image_info.image_height * image_info.image_width; i++)
      fscanf(input_file, "%d", &original_pixel_values[i]);

    return;
  }

// Transpose image using array
void Image::transpose_image()
  {
    read_pixel_values();
    int i;
    for (i = image_info.image_width * image_info.image_height - 1; i >= 0; i--)
      fprintf(output_file, "%d\n", original_pixel_values[i]);

    free(original_pixel_values);
    return;
  }

// Mirror image using array
void Image::mirror_image()
  {
    read_pixel_values();
    int row, column;
    for (row = 0; row < image_info.image_height; row++)
      {
        for (column = image_info.image_width - 1; column >= 0; column--)
          fprintf(output_file, "%d\n", original_pixel_values[row*image_info.image_width + column]);
      }

    free(original_pixel_values);
    return;
  }

// Unsharp mask
void Image::unsharp_mask()
  {
    float multiplying_constant;
    printf("\nEnter multiplying constant: ");
    scanf("%f", &multiplying_constant);
    while (multiplying_constant < 1)
      {
        printf("Error.  Multiplying constant must be greater than or equal to 1.  Try again.\nEnter multiplying constant: ");
        scanf("%f", &multiplying_constant);
      }

    read_pixel_values();

    gaussian_blur();                                                            // Apply blur mask int pixel_value; int row, column;
    int row, column, value;
    for(row = 0; row < image_info.image_height; row++)
      for (column = 0; column < image_info.image_width; column++)
        {
          value = (int)(original_pixel_values[row*image_info.image_width + column] + multiplying_constant*(original_pixel_values[row*image_info.image_width + column] - blurred_pixels[row*image_info.image_width + column]));
          
          // Cap values at 0 and 255
          if (value < 0)
            value = 0;
          else if (value > 255)
            value = 255;
          // printf("%d  %d %d\n", original_pixel_values[row*image_width + column], blurred_pixels[row*image_width + column], original_pixel_values[row*image_width + column] + multiplying_constant*(original_pixel_values[row*image_width + column] - blurred_pixels[row*image_width + column]));
          // fprintf(output_file, "%d\n", (original_pixel_values[row*image_width + column] * multiplying_constant) - blurred_pixels[row*image_width + column]);
          fprintf(output_file, "%d\n", value);
          // fprintf(output_file, "%d\n", blurred_pixels[row*image_width + column]);
        }

    free(original_pixel_values);
    free(blurred_pixels);
    return;
  }

// Create gaussian blur
void Image::gaussian_blur()
  {
    blurred_pixels = (int *) malloc(sizeof(int) * image_info.image_height * image_info.image_width);
    int pixel_index;
    float sum;
    int row, column, i;
    int max = 0;

    // Set 4 borders to original pixels
    for(i = 0; i < image_info.image_width; i++)
      blurred_pixels[i] = original_pixel_values[i];
    for(i = image_info.image_width*(image_info.image_height-1); i < image_info.image_width*image_info.image_height; i++)
      blurred_pixels[i] = original_pixel_values[i];
    for(i = 1; i < image_info.image_height-1; i++)
      {
        blurred_pixels[i*image_info.image_width] = original_pixel_values[i*image_info.image_width];
        blurred_pixels[i*image_info.image_width+image_info.image_width-1] = original_pixel_values[i*image_info.image_width];
      }

    // Apply blur mask
    for(row = 1; row < image_info.image_height-1; row++)
      {
        for (column = 1; column < image_info.image_width-1; column++)
          {
            sum = 0;
            sum += ((float)1/(float)16) * (float)original_pixel_values[((row-1) * image_info.image_width) + (column-1)];
            sum += ((float)1/(float)8) * (float)original_pixel_values[((row-1) * image_info.image_width) + (column)];
            sum += ((float)1/(float)16) * (float)original_pixel_values[((row-1) * image_info.image_width) + (column+1)];
            sum += ((float)1/(float)8) * (float)original_pixel_values[((row) * image_info.image_width) + (column-1)];
            sum += ((float)1/(float)4) * (float)original_pixel_values[((row) * image_info.image_width) + (column)];
            sum += ((float)1/(float)8) * (float)original_pixel_values[((row) * image_info.image_width) + (column+1)];
            sum += ((float)1/(float)16) * (float)original_pixel_values[((row+1) * image_info.image_width) + (column-1)];
            sum += ((float)1/(float)8) * (float)original_pixel_values[((row+1) * image_info.image_width) + (column)];
            sum += ((float)1/(float)16) * (float)original_pixel_values[((row+1) * image_info.image_width) + (column+1)];
            blurred_pixels[row*image_info.image_width + column] = sum;
            
            if(sum > max) // Find max value for scaling
              max = sum;
          }
      }

    // Scale blured values
    for(row = 1; row < image_info.image_height-1; row++)
      for (column = 1; column < image_info.image_width-1; column++)
        blurred_pixels[row*image_info.image_width + column] = blurred_pixels[row*image_info.image_width + column]*(255/max);

    return;
  }

// Operator overloading: image + image
void Image::operator+(Image image2)
  {
    this->read_pixel_values();

    int threshold;
    printf("\nEnter a threshold value (must be between 0 and 255 inclusive): ");
    scanf("%d", &threshold);
    while (threshold < 0 || threshold >= 255) // Make sure threshold value is in valid range
      {
        printf("\nError.  Your threshold value must be between 0 and 254 inclusive (must be less than max gray value). Try again.\n"
               "Enter a threshold value: ");
        scanf("%d", &threshold);
      }
    this->binary_threshold(threshold);

    for (int i = 0; i < image_info.image_height * image_info.image_width; i++) {
      fprintf(output_file, "%d\n", original_pixel_values[i] + binary_threshold_pixels[i]);
    }
  }

// Operator Overloading: constant + image
void operator+(int constant, Image& user_image)
  {
    user_image.read_pixel_values();

    for (int i = 0; i < user_image.image_info.image_height * user_image.image_info.image_width; i++) {
      fprintf(user_image.output_file, "%d\n", user_image.original_pixel_values[i] + constant);
    }
  }

// Operator overloading: image + constant
void Image::operator+(int constant)
  {
    this->read_pixel_values();

    for (int i = 0; i < image_info.image_height * image_info.image_width; i++) {
      fprintf(output_file, "%d\n", original_pixel_values[i] + constant);
    }
  }

/* end of file */