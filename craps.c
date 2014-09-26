/* Jordan Buttkevitz
 * jpb80
 * CS449 MW 1pm */

#include <stdio.h>
#include <string.h>

char line[100];
char name[100];
char response[100];

int point_round(int point, int current_dice_total) {
   if (current_dice_total == point) { 
      return 1; // player won 
   } else  if (current_dice_total == 7) {
      return 7;  // player lost
   } else {  
      return 0; //continue rolling
   } 
} //end point

int random() {
      int high = 6;
      int low = 1; 
      int dice_1;
      int dice_2;
      int dice_total;

      dice_1 = rand() % (high - low + 1) + low;
      dice_2 = rand() % (high - low + 1) + low;
      dice_total = dice_1 + dice_2;
      printf("You have rolled %d + %d = %d\n", dice_1, dice_2, dice_total); 
      return(dice_total);
} //end random

int prompt() {
   int play_response;
   int quit_response;
   
   while (1) { 
      printf("%s, would you like to Quit or Play? ", name);
      fgets(line, sizeof(line), stdin);
      sscanf(line,"%s", &response);
  
      play_response =  strcmp("Play", response);
      quit_response = strcmp("Quit", response);
   
      if (play_response == 0)  {
	 return 1;
      } else if (quit_response == 0) {
	 return 0; 
      } else { 
	 printf("\"%s\" is an invalid response. Please answer with Play or Quit.\n",response); 
	 continue;
      }
   } 
}//end prompt

int play_again_prompt() {
   char play_again[100]; 
   int no_play;
   int yes_play;

   while (1) {
      printf("Would you like to play again? "); 
      fgets(line, sizeof(line),stdin);
      sscanf(line, "%s", &play_again);
     
      no_play = strcmp("No", play_again);
      yes_play = strcmp("Yes",play_again);
      
      if (no_play == 0) {
	 return 0;
      } else if (yes_play == 0) { 
	 strncpy(play_again,"",100); 
	 return 1;
      } else {	
	 printf("\"%s\" is an invalid response. Please answer with Yes or No.\n",play_again);
      }
   }
}//end play_again_prompt



int main() {
   int point;
   int control;
   int dice_total; 
   int response_play;

   srand((unsigned int)time(NULL)); //seeding random generation   
   printf("Welcome to Jon's Casino!\n");	  
   printf("Please enter your name: ");  
   fgets(line, sizeof(line), stdin);
   sscanf(line, "%s", &name);
   
   control = prompt();

   while (control) {
      int point_round_trigger;
      int point_round_return;
      
      dice_total = random();  
      switch(dice_total) {
	 case  2:
	       printf("You've been defeated!\n");
	       break;
	 case  3:
	       printf("You've been defeated!\n"); 
	       break;
	 case  12:
	       printf("You've been defeated!\n");
	 case  7:
	       printf("You win!!\n");
	       break;
	 case  11:
	       printf("You win!!\n");
	       break;
	 default: 
	       point_round_trigger = 1;
	       point = dice_total;
	       while (1) { 
		  int curr_dice_total; 
		  
		  curr_dice_total = random(); // a new roll..compare this value to the point roll value   
		  point_round_return = point_round(point, curr_dice_total);
	       	  if (point_round_return == 7) {
		     printf("You've been defeated!\n"); 
		     break;
		  } else if (point_round_return == 1) {
		     printf("Victory is yours, you win!\n"); 
		     break; 
		  } else { 
		     continue;
		  } 
	       } 
      }//end switch

      response_play = play_again_prompt(); 
      if (response_play == 1) { 
	 control = 1;
      }	else {
	 control = 0; 
      } 
   }//end while 
   printf("Good bye, %s\n", name);
   return(0);
}//end of main
