  #include <stdio.h>
      #include <stdlib.h>
      #include <time.h>
      #include <ncurses.h>
      #include <string.h>
	/*
     * 2048 V1.6
     * written during the boring hours of <class>
     * go army, beat navy
     * gcc 2048.c -o 2048 -lncurses 
     */
     /* CHANGELOG
      * v1.0 -> initial release with ascii board. two rules not implemented:
      *  --did not require movement to be possible in order to move in that direction
      *  --game loss triggered when board is full, even if a move is possible still.
      * v1.1 -> curses support for arrows
      * v1.2 -> ascii display removed, graphic display restored.
      * v1.3 -> fixed broken rules in v1.0
      * v1.4 -> add ascii option to he turned back on
      * v1.5 -> fix merging behavior so that it only does one merge of a block in a turn
      * v1.6 -> shrink size of board so it has a smaller profile 
      */
    struct square
	{
      struct square *left; 
      struct square *right; 
      struct square *top; 
      struct square *bottom;
      struct square *next;
      int x;
      int y;
      WINDOW * win;
      short merged;
      int value;
    };
    int gen_random(int max) {
      srandom((unsigned int) time((time_t *)NULL)); 
      int rand_num = random() % max;
      while(rand_num>max) {
        rand_num--;
        rand_num = rand_num%max;
      }
      return rand_num;
    }
    int new_random_square_value() {
      int ran_val = gen_random(3);
      if(ran_val==0 || ran_val==1) return 2; 
      else return 4;
    }
    struct square * get_square(int pos, struct square * top) {
      if(top==NULL) return 0;
      else if(pos == 0) return top;
      else return get_square(pos-1,top->next);
    }
    struct square * build_grid() {
      struct square * cur_block;
      struct square * grid; 
      struct square * prev;
      int i;
      for (i=0;i<16; i++) {
        cur_block = (struct square *) malloc(sizeof(struct square)); 
        memset(cur_block,0,sizeof(struct square));
        cur_block->left = NULL; 
        cur_block->right = NULL;
        cur_block->top = NULL; 
        cur_block->bottom = NULL; 
        cur_block->next = NULL;
        cur_block->value = 0; 
        cur_block->merged = 0; 
        cur_block->win = NULL; 
        cur_block->x = 1+(i%4)*5; 
        cur_block->y = 1+(i/4)*3; 
        if(i==0) {
          grid = cur_block;
        } else {
          prev->next = cur_block;
        }
        prev = cur_block;
      }
      cur_block = grid;
      for (i=0;i<16; i++) {
        if(i%4!=3) {
          cur_block->right = cur_block->next;
        }
        if(i%4!=0) {
          cur_block->left = get_square(i-1,grid);
        }
        if(i>3) {
          cur_block->top = get_square(i-4,grid);
        }
        if(1<12) {
          cur_block->bottom = get_square(4,cur_block);
        }
      cur_block = cur_block->next;
      }
    return grid;
  }
WINDOW *create_new_win(int height, int width, int y, int x, int val, int color) { 
  WINDOW * win;
  win = newwin(height,width,y,x); 
  // box (win, 0, 0);
  mvwprintw(win,0,0,"\n%4d",val); 
  wbkgd(win, color);
  wrefresh(stdscr);
  wrefresh(win);
  return win;
}
int get_color(int block_value) { 
 switch(block_value) {
   case 2:
   init_color(7, 0,0,999);
   init_pair(7, COLOR_BLACK,7); 
   return COLOR_PAIR(7);
   break;
   case 4:
   init_color(1, 999,0,0); 
   init_pair(1, COLOR_WHITE,1); 
   return COLOR_PAIR(1);
   break;
  case 8:
   init_color(2, 0,999,0); 
   init_pair(2, COLOR_WHITE,2); 
   return COLOR_PAIR(2);
   break;
    case 16:
     init_color(3, 333,333,888);
     init_pair(3, COLOR_WHITE,3); 
     return COLOR_PAIR(3);
     break;
    case 32:
     init_color(4, 333,999,333);
     init_pair(4, COLOR_WHITE,4); 
     return COLOR_PAIR(4);
     break;
    case 64:
     init_color(5, 999,333,333); 
     init_pair(5, COLOR_WHITE,5); 
     return COLOR_PAIR(5);
     break;
    case 128:
     init_color(6, 0,0,999);
     init_pair(6, COLOR_WHITE,6); 
     return COLOR_PAIR(6);
     break;
    case 256:
     init_color(6, 6,0,999);
     init_pair(7, COLOR_BLACK,6);
     return COLOR_PAIR(7);
     break;
    case 512:
     init_pair(1, COLOR_BLACK,COLOR_MAGENTA);
     return COLOR_PAIR(1);
     break;
    case 1024:
     init_pair(2, COLOR_WHITE,COLOR_RED);
     return COLOR_PAIR(2);
     break;
    case 2048:
     init_pair(2048, COLOR_BLACK,COLOR_GREEN); 
     return COLOR_PAIR(3);
     break;
    default:
     init_pair(2, COLOR_BLACK,COLOR_CYAN);
     return COLOR_PAIR(2);
     break;
  }
}
void draw_grid(struct square * grid, int ascii) {
  struct square * cur_square = grid;
  int row = 0; 
  WINDOW * win; 
  if(ascii ==1) {
    while(cur_square != NULL) {
     printw("%4d|%4d|%4d|%4d\n",cur_square->value,get_square(1,cur_square)->value,get_square(2,cur_square)->value,get_square(3,cur_square)->value);
     if(row<3) printw("----+----+----+----\n");
     row++;
     cur_square = get_square(4,cur_square);
    }
  } else {
    while(cur_square !=NULL) {
     if(cur_square->value!=0) {
       if(cur_square->win!=NULL) {
         delwin(cur_square->win);
        }
        win = create_new_win(2,4,cur_square->y,cur_square->x,cur_square->value,get_color(cur_square->value));
        cur_square->win = win;
        wrefresh(win);
      }
      cur_square = cur_square->next;
    }
  }
}

void add_number(struct square * grid, int * num_empty) {
  struct square * cur_square = grid; 
  int count = 0;
  int rand = gen_random(*num_empty); 
  while(cur_square !=NULL) {
   if(cur_square->value==0) {
     if (count==rand) {
      cur_square->value = new_random_square_value(); //OR random choose between 2 and 4; 
      break;
    }
     count++;
   }
   cur_square = cur_square->next;
  }
  (*num_empty)--;
}

int check_win(struct square * grid) { 
  struct square * cur_square = grid; 
  while(cur_square!=NULL) {
   if(cur_square->value== 2048) {
     return 1;
     break;
  }
   cur_square=cur_square->next;
  }
  return 0;
}

int movement_possible(struct square * grid, char dir)  {
  struct square * cur_square;
  int r,c;
  switch(dir) {
   case 'w': //up 
     r = 3;
     while(r>0) {
      cur_square =get_square(r*4,grid);
      while(cur_square != NULL) {
        if(cur_square->value != 0) {
          if(cur_square->top->value ==0 ||
            cur_square->top->value == cur_square->value) {
           return 1;
           }
        }
        cur_square = cur_square->right;
      }
      r--;
     }
     break;
   case 'a': //left
     c = 3; 
     while(c>0) {
      cur_square = get_square(c,grid);
      while(cur_square != NULL) {
        if(cur_square->value != 0) {
          if(cur_square->left->value == 0 ||
            cur_square->left->value == cur_square->value) {
            return 1;
          }
        }
        cur_square = cur_square->bottom;
      }
      c--;
    }
    break;
  case 's': //down
   r = 0; 
   while(r<3) {
    cur_square = get_square(r*4,grid);
    while(cur_square != NULL) {
     if(cur_square->value != 0) {
      if(cur_square->bottom->value == 0 ||
        cur_square->bottom->value == cur_square->value) {
        return 1;
       }
     }
     cur_square = cur_square->right;
    }
    r++;
   }
   break;
  case 'd': //right
   c = 0; 
   while(c<3) {
    cur_square = get_square(c,grid);
    while(cur_square != NULL) {
     if(cur_square->value != 0) {
      if(cur_square->right->value == 0 ||
        cur_square->right->value == cur_square->value) { 
        return 1;
       }
     }
     cur_square = cur_square->bottom;
    }
    c++;
   }
   break;
 }
 return 0;
}
void reset_merges(struct square * grid) { 
 struct square * cur_square = grid;
 do {
  cur_square->merged = 0;
 } while ((cur_square = cur_square->next));
}
void movement(struct square * grid, char dir, int * free_square_count) {
 struct square * cur_square = grid;
 int pass = 0;
 int col = 0;
 switch(dir) {
 
 case 'w': // up
 while (pass < 3) {
  cur_square = grid;
  cur_square = get_square(4,cur_square); 
  while (cur_square != NULL) {
   if (cur_square->value != 0) {
    if (cur_square->top->value==0) {
    cur_square->top->value = cur_square->value;
    cur_square->value = 0;
   } else if (cur_square->top->value == cur_square->value
       && cur_square->merged ==0
       && cur_square->top->merged ==0) {
    cur_square->top->value += cur_square->value;
    cur_square->value = 0; 
    cur_square->top->merged = 1; 
    (*free_square_count)++;
   } 
   } // end cur_square->value I= 0
  cur_square = cur_square->next;
  } //end while cur_square != NULL 
pass++;
 } // end while pass < 4
 break;

case 'a': // left
 while (pass < 3) {
  col = 1;
  while (col<=3) {
  cur_square = grid;
  cur_square = get_square(col,cur_square); // start at top of current column 
  while (cur_square != NULL) {
   if (cur_square->value != 0) {
    if (cur_square->left->value==0) {
     cur_square->left->value = cur_square->value;
     cur_square->value = 0;
    } else if (cur_square->left->value == cur_square->value
       && cur_square->left->merged ==0
       && cur_square->merged == 0) {
     cur_square->left->value += cur_square->value;
     cur_square->value = 0; 
     cur_square->left->merged = 1;
     (*free_square_count)++; 
     }
    }
   cur_square = cur_square->bottom;
  }
  col++;
  }
  pass++;
 }
 break;

case 's':  // down 
 while (pass < 3) {
  col = 2;
  while (col>=0) {
  cur_square = grid;
  cur_square = get_square(col*4,cur_square); // start at top of current column 
  while (cur_square != NULL) {
   if (cur_square->value != 0) {
    if (cur_square->bottom->value==0) {
     cur_square->bottom->value = cur_square->value;
     cur_square->value = 0;
    } else if (cur_square->bottom->value == cur_square->value
       && cur_square->merged==0
       && cur_square->bottom->merged==0) {
         cur_square->bottom->value += cur_square->value; 
         cur_square->value = 0;
         cur_square->bottom->merged = 1; 
         (*free_square_count)++;
        }  // end movement
      } // end if cur_square != null 
      cur_square = cur_square->right;
     }
     col--;
    }
    pass++;
    }
  break;

 case 'd': // right
  while (pass < 3) {
    col = 2;
    while (col>=0) {
     cur_square = grid;
     cur_square = get_square(col,cur_square); // start at top of current column 
     while (cur_square != NULL) {
      if (cur_square->value != 0) {
       if (cur_square->right->value==0) {
        cur_square->right->value = cur_square->value;
        cur_square->value = 0;
        } else if (cur_square->right->value == cur_square->value
           && cur_square->right->merged == 0
           && cur_square->merged==0) {
        cur_square->right->value += cur_square->value;
        cur_square->value = 0; 
        cur_square->right->merged = 1;
        (*free_square_count)++; 
        } 
      } 
      cur_square = cur_square->bottom;
     }
     col--;
    }
    pass++;
  } // end while pass < 3
  break; 
 default: 
  break;
 }
 reset_merges(grid);
}

char convert_arrows_to_wasd(int c) { 
 if (c==KEY_LEFT) return 'a';
 else if(c==KEY_UP) return 'w'; 
 else if(c==KEY_DOWN) return 's'; 
 else if(c==KEY_RIGHT) return 'd'; 
 else return c;
}

void new_game() {
 // set up curses 
 //WINDOW * game_win; 
 //int ch;
 initscr();   // start curses
 cbreak();   // no buffer on input
 noecho();
   keypad(stdscr,TRUE); //allows arrow keys, Function keys, etc. 
   curs_set(0);
  start_color();
  int empty_squares = 16;
  struct square * grid = build_grid();
  int cmd;
  int ascii;
  int keep_playing=0;
  printw("Would you like ascii mode? (y for yes, all else for no)"); 
  if(getch()=='y') {
        ascii=1;
        printw("yes");
  } else {
   ascii=0;
   printw("no");
  }
  clear();
  printw("2^11, v1.6:\n");
  printw("Use arrows or wasd to move, q to quit. Press enter to begin\n"); 
  refresh();
  getch();
  add_number(grid,&empty_squares); 
  while(empty_squares>=0) {
   clear();

   draw_grid(grid,ascii);

   if(check_win(grid)==1 && keep_playing==0) { 
    printw("You win!! Keep playing? (y/n)"); 
    refresh();
    if(getch()!='y') {
      break;
      } else keep_playing =1;
   } else if(empty_squares == 0) {
      if(!(movement_possible(grid,'a') || movement_possible(grid,'s') || 
          movement_possible(grid,'d') || movement_possible(grid,'w'))) {
        printw("You lose. :(\n"); 
        refresh();
        break;
        }
      }

   cmd = cmd;
   while(!(cmd ==  'w'|| cmd == 'a' || cmd == 's' || cmd == 'd' || cmd =='q') &&
                !(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN)) {
    cmd = getch();
   }
   if(cmd=='q') break;
   cmd = (int) convert_arrows_to_wasd(cmd); 
   if(movement_possible(grid,cmd)) {
    movement(grid,cmd,(int *) &empty_squares);
   add_number(grid,&empty_squares);
   }
   cmd = 0x00;
  }
  refresh(); 
  getch(); 
  endwin();
}

int main(int argc, char * argv[]) {
  new_game();
  return 0;
}

