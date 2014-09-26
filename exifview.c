#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int long current_position; 
int long next_tag;
char camera_manufacturer[100];
char camera_model[200];
int final_height;
int final_width;
int short final_iso;
float final_exposure;
float final_fstop;
unsigned int final_focal;
char date_taken[100];

struct exif_header { 
   unsigned char start_marker[2]; 
   unsigned char app1_marker[2];
   unsigned char app1_length[2];
   unsigned char exif_str[4];
   unsigned char null_term[2];
   unsigned char endian[2];
   unsigned char version[2];
   unsigned int offset;
}header; 

struct tiff_tag {
   unsigned char tag_id[2];
   unsigned short data_type;
   unsigned int item_nu; 
   unsigned int offset_value;
} descript_tags;

/*header_tags reads the first 20 bytes of EXIF header and checks for compatibility. 
 * Note: the integer values are the equivalent hexadecimal values**/
int header_tags(int current_position, int count, FILE* input_file, 
							 struct exif_header header) {   
   int i;
   fseek(input_file, next_tag, SEEK_SET);
   fread(&header, sizeof(header), 1, input_file);  
   if (header.start_marker[0] != 255 && header.start_marker[1] != 216) { //0xFFD8
      printf("File not compatible.\n");
      exit(8); 
   }
   if (header.app1_marker[0] != 255 && header.app1_marker[1] != 225) { //0xFFE1
      printf("File not compatible.\n");
      exit(8);
   }
   if (header.endian[0] != 73) { //0x49: I 
      printf("File incompatible with big endian.\n");
      exit(8);
   }
   if (header.exif_str[0] != 69) {  // 0x45: E
      printf("File does not support EXIF.\n"); 
      exit(8); 
   }
 
   next_tag = current_position + 12;
   return next_tag;
}//end header_tags

//tiff_tags moves the file pointer to the current position and returns the next position as an integer.   
int tiff_tags(int current_position, FILE* input_file) { 
   while (!feof(input_file)) {
      fseek(input_file,current_position, SEEK_SET);
      if (fread(&descript_tags, sizeof(descript_tags), 1, input_file) != 1 ) {
	 fprintf(stderr, "File Read Error\n"); 
	 exit(8); 
      } 
      next_tag = ftell(input_file) + 12; 
      return next_tag;
   }
   return -1;
}//end tiff_tags


//exif_tags moves the current position + 12. 
void exif_tags(int current_position, FILE* input_file) {
   fseek(input_file, current_position + 12,  SEEK_SET);    
}//end exif_tags


//check_tiff_tagID compares the each descript_tags.tagID array elements to the desired tag IN DECIMAL not hex values. 
int check_tiff_tagID(unsigned char tagID[], unsigned int first_byte, 
						      unsigned int sec_byte)  { 
  if (tagID[0] == first_byte && tagID[1] == sec_byte) {
      return 2;
   } else {
      return 0;
   }
}//end check_tiff_tagID 


/*exif_tags_loop accepts the current position of the file pointer and uses fread to obtain the descript_tag offset value
 * that contains the count of tags in the EXIF subblock.  The function also compares the current exif tagID to the desired
 * tags in decimal values. The offset value of those found exif tags are stored in variables. Note: tagID values are reversed
 * since the EXIF is in little indian (II).**/ 
void exif_tags_loop(int current_position, FILE* input_file) {
   unsigned short exifsub_count;
   int curr_pos;
   int new_pos;
   exif_tags(current_position, input_file);   
   fread(&exifsub_count, sizeof(exifsub_count), 1, input_file); //exifsub_count == count of exif tags in sub block     
   new_pos = ftell(input_file); 
      
   while (exifsub_count > 0) {    
     int first_byte;
      int sec_byte;
      int width;
      int height;
      int ISO;
      int exposure;
      int fstop;
      int focal;
      int date;
      int item; 
     
      curr_pos  = tiff_tags(new_pos, input_file) - 12;  
      new_pos = curr_pos;

      height = check_tiff_tagID(descript_tags.tag_id, 3 , 160 ); //0x03a0 height in pixels 
      if (height  ==  2) {
	 final_height = descript_tags.offset_value; 
      }  
      
      width = check_tiff_tagID(descript_tags.tag_id, 2, 160); //0x02a0 width in pixels 
       if (width  ==  2) {
	 final_width = descript_tags.offset_value;  
      } 
      
      ISO = check_tiff_tagID(descript_tags.tag_id, 39, 136); //0x2788 
      if (ISO  ==  2) {
	 final_iso = descript_tags.offset_value;  
      } 

      exposure = check_tiff_tagID(descript_tags.tag_id, 154, 130); //0x9a82
      if (exposure  ==  2) {	
	 int unsigned test[2]; 
	 fseek(input_file, descript_tags.offset_value + 12, SEEK_SET); 
	 fread(&test, sizeof(test), 1, input_file);  
	 final_exposure= 1/((float)test[0]/(float)test[1]); 
      }
      
      fstop = check_tiff_tagID(descript_tags.tag_id, 157, 130);//0x9d82
      if (fstop  ==  2) { 
	 int unsigned test[2]; 
	 fseek(input_file, descript_tags.offset_value + 12, SEEK_SET); 
	 fread(&test, sizeof(test), 1, input_file);  
	 final_fstop = (float)test[0]/(float)test[1]; 
      } 
        
      focal = check_tiff_tagID(descript_tags.tag_id, 10, 146); //0x0a92
      if (focal  ==  2) {
	 int unsigned test[2]; 
	 fseek(input_file, descript_tags.offset_value + 12, SEEK_SET); 
	 fread(&test, sizeof(test), 1, input_file);  
	 final_focal = test[0]/test[1];  
      }

      date  = check_tiff_tagID(descript_tags.tag_id, 3, 144); //0x0390
      if (date  ==  2) {
	 fseek(input_file, descript_tags.offset_value + 12, SEEK_SET);
	 fread(&date_taken, sizeof(date_taken), 1, input_file); 
      }
      
      exifsub_count--;    
  }//end while 
} //end exif loop

  
int main(int argc, char *argv[]) { 
   int new_pos;  
   unsigned short count;
  
   //Loading the exif file
   FILE* input_file = fopen (argv[1], "rb");
   if (input_file == NULL) {
      fprintf(stderr,"Invalid or missing file\n");
      exit(8);
   }

   //Read input file one time and push those bytes into entire structure tiff_tag header.  
   fseek(input_file, 0, SEEK_SET);
   int read_byte;
   read_byte = fread(&header, sizeof(header), 1, input_file);
   if (read_byte != 1) {
      printf("File read failed...aborting\n");
      exit(8);
   } 
  
   //Seek to 20th byte from beginning of file to capture the number of tiff tags in upcoming section.
   fseek(input_file, 20, SEEK_SET);
   fread(&count, sizeof(count), 1, input_file); 
   
   //Retreiving the file's current pointer position.
   current_position = ftell(input_file); 
   next_tag = header_tags(0,count, input_file, header);
   new_pos = current_position;
 
   //Search through the count tiff tags after the header until exif subblock is found. 
   while (count > 0) {   
      int manufact;
      int model;
      int exif_add;
      int item;
      int position;
      int curr_pos;
      curr_pos  = tiff_tags(new_pos, input_file) - 12;
      new_pos = curr_pos;
      
      manufact = check_tiff_tagID(descript_tags.tag_id, 15 ,1 ); //0x0f10 manufacturer string 
      if (manufact  ==  2) { 
	 item = descript_tags.item_nu;
	 if (item  > 4) {  
	    position = descript_tags.offset_value; 
	    fseek(input_file, position + 12, SEEK_SET);
	    fread(&camera_manufacturer, sizeof(camera_manufacturer), 1, input_file);	   
	 } else {
	    fseek(input_file, curr_pos, SEEK_SET);
	    fread(&camera_manufacturer, sizeof(camera_manufacturer), 1, input_file);
	 }
      }   
      
      model = check_tiff_tagID(descript_tags.tag_id,16 ,1); //0x0110 Camera model  
      if (model  ==  2) {
	 item = descript_tags.item_nu; 
	 if (item  > 4) {	
	    position = descript_tags.offset_value; 
	    fseek(input_file, position + 12, SEEK_SET);
	    fread(&camera_model, sizeof(camera_model), 1, input_file); 
	 } else {
	    fread(&camera_model, sizeof(camera_model), 1, input_file);
	    
	 }
      } 
      count--;
      
      exif_add = check_tiff_tagID(descript_tags.tag_id, 105, 135); //0x6987 EXIF sub-block 
      if (exif_add ==  2) {
	 curr_pos = descript_tags.offset_value; // enter the exif tag search loop and print remaining values.  
	 exif_tags_loop(curr_pos, input_file); 
	 break;
      } 
   }//end while

   fclose(input_file);
  
   //Printing ouput
   printf("Manufacturer:  %s\n", camera_manufacturer);
   printf("Model:         %s\n", camera_model);
   printf("Exposure Time: 1/%.0f second\n", final_exposure);
   printf("F-stop:        f/%.1f\n", final_fstop);
   printf("ISO:           ISO %hd\n", final_iso);
   printf("Date Taken:    %s\n", date_taken);
   printf("Focal Length:  %llu mm\n", final_focal);
   printf("Width:         %d pixels\n", final_width);
   printf("Height:        %d pixels\n", final_height); 

   return (0);
}//end exifviewer
