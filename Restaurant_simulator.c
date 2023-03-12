#include "a1.h"

Restaurant* initialize_restaurant(char* name) {
    Restaurant* r = malloc(sizeof(Restaurant));
    Queue* q = malloc(sizeof(Queue));
    Menu* m = malloc(sizeof(Menu));

    r->name = name;
    m = load_menu(MENU_FNAME); 
    r->menu = m; // This probably caused some issues, initializing m
    // r->menu = load_menu(MENU_FNAME);
    
    r->num_completed_orders = 0;
    r->num_pending_orders = 0;
    
    q->head = NULL;
    q->tail = NULL;
    
    r->pending_orders = q;
    
    return r;
}


Menu* load_menu(char* fname) {
    Menu* m = malloc(sizeof(Menu));

    char** all_codes_a = (char**)malloc(sizeof(char*));
    char* single_code = (char*)malloc(sizeof(char)*(ITEM_CODE_LENGTH));
    all_codes_a[0] = single_code;
    
    char** all_names_a = (char**)malloc(sizeof(char*));
    char* single_name = (char*)malloc(sizeof(char)*(MAX_ITEM_NAME_LENGTH));
    all_names_a[0] = single_name;
    
    double* all_costs_a = (double*)malloc(sizeof(double));
    char single_cost[100];

    for (int i=0;i<100;i++) {
        single_cost[i] = 0;
    }
    
    FILE* fp = fopen(fname, "r");
    char* line = NULL;
    size_t line_len = 0;
    int l_count = 0;

    int count = 0;
    int first_letter = 0;
    int j = 0;
    int sep_num = 0;
    
    while (getline(&line, &line_len, fp) != -1) {
        if (l_count != 0) {
            all_codes_a = realloc(all_codes_a, sizeof(char*)*(l_count+1));
            single_code = (char*)malloc(sizeof(char)*(ITEM_CODE_LENGTH));
            all_codes_a[l_count] = single_code;
            
            all_names_a = realloc(all_names_a, sizeof(char*)*(l_count+1));
            single_name = (char*)malloc(sizeof(char)*(MAX_ITEM_NAME_LENGTH));
            all_names_a[l_count] = single_name;
            
            all_costs_a = realloc(all_costs_a, sizeof(double)*(l_count+1));
            for (int i=0;i<100;i++) {
                    single_cost[i] = 0;
                }
        }
        
        count = 0;
        first_letter = 0;

        for (int i = 0; line[i]; i++)
            count++;
        
        j = 0;
        sep_num = 0;
        

        //Problems: 
        //the item code is getting weird symbols in front of them

        //Going through the line now
        for (int i=0; i<=count; i++) {
            if (line[i] != MENU_DELIM[0] && i!=count) {
                if (first_letter == 0) {
                    if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r') {
                        first_letter = 1;
                        i--;
                    }
                } else if (first_letter == 1) {
                    if ((line[i] == ' ' || line[i] == '\t' || line[i] == '\r') && sep_num == 2) {
                        first_letter = 2;
                    } else {
                        if (sep_num == 0) {
                            single_code[j] = line[i];
                        } else if (sep_num == 1) {
                            single_name[j] = line[i];
                        } else if (sep_num == 2) {
                            if (j!= 0) {
                                single_cost[j-1] = line[i];
                            }
                        }
                        j++; // utf-8' codec cannot be decode here
                    }
                }
            } else {
                j = 0;
                sep_num++;
            }

            if (i == count) {
                all_costs_a[l_count] = atof(single_cost);
                for (int k=0;k<100;k++) {
                    single_cost[k] = 0;
                }
            }
        }
        l_count++;
    }
  
    // printf("day %d", l_count); //l_count is equal to 7 tested with other number of items this is because of the location of the cursor, problem resolved
    m->num_items = l_count;
    m->item_codes = all_codes_a;
    m->item_names = all_names_a;
    m->item_cost_per_unit = all_costs_a;
    free(line);
    fclose(fp);

    return m; // utf-8' codec cannot be decode here
}

// problem with it not computing the arithmetics and also incorrect signage, copying not working properly, building the incorrect order
Order* build_order(char* items, char* quantities) {
    Order* o = malloc(sizeof(Order));
    char** codes_a = (char**)malloc(sizeof(char*));
    codes_a[0] = malloc(sizeof(char)*ITEM_CODE_LENGTH);
    
    int item_count = 1;
    int letter_count = 0;
    
    //Copying over the item codes
    
    for (int i=0; items[i]!='\0'; i++) {
        if (i!=0 && i%(ITEM_CODE_LENGTH-1) == 0) {
            item_count++;
            codes_a = realloc(codes_a, sizeof(char*)*(item_count));
            codes_a[item_count-1] = malloc(sizeof(char)*ITEM_CODE_LENGTH);
            letter_count = 0;
        }
      
        codes_a[item_count-1][letter_count] = items[i];
        letter_count++;
    }

    int* quant_a = (int*)malloc(sizeof(int)*item_count);
    char single_quant[MAX_ITEM_QUANTITY_DIGITS];
    for (int k=0;k<MAX_ITEM_QUANTITY_DIGITS;k++) {
        single_quant[k] = 0;
    }
    int num_count = 0;
    int quant_count = 0;
    
    //Copying over the quantities

    //Sometimes the copying doesn't work D:
    //I suspect it has to do with the ending condition but I haven't figured it out yet :|
  
    for (int i=0; quantities[i]!='\0';i++) {
        if (quantities[i] != MENU_DELIM[0]) {
            single_quant[num_count] = quantities[i];
            num_count++;
        } else {
            if ((num_count+1) != MAX_ITEM_QUANTITY_DIGITS) {
                single_quant[num_count] = '.';
            }
            quant_a[quant_count] = (int)atof(single_quant);
            
            //check memory here later
            for (int k=0;k<MAX_ITEM_QUANTITY_DIGITS;k++) {
                single_quant[k] = 0;
            }
          
            num_count = 0;
            quant_count++;
        }
    }

  
    o->item_codes = codes_a;
    o->num_items = item_count;
    o->item_quantities = quant_a;
        
    return o;
}

void enqueue_order(Order* order, Restaurant* restaurant) {
    QueueNode* new_order = malloc(sizeof(QueueNode));
    
    restaurant->pending_orders->tail->next = new_order;
    new_order->next = NULL;
    new_order->order = order;
    restaurant->pending_orders->tail = new_order;
    
    (restaurant->num_pending_orders)++;
    
}

Order* dequeue_order(Restaurant* restaurant) {
    Order* d_order = restaurant->pending_orders->head->order;
    QueueNode* old_h = restaurant->pending_orders->head;
    QueueNode* new_h = restaurant->pending_orders->head->next;
    old_h->next = NULL;
    free(old_h->order->item_codes);
    free(old_h);
    
    restaurant->pending_orders->head = new_h;
    (restaurant->num_pending_orders)--;
    (restaurant->num_completed_orders)++;
    
    return d_order;
}

double get_item_cost(char* item_code, Menu* menu) {
    int pos;
    //Looking through the array of pointers to codes
    for (int i=0; i<(menu->num_items); i++) {
        //Looking through individual codes
        int same_letter = 0;
        for (int j=0; i<(ITEM_CODE_LENGTH-1); j++) {
            if ((menu->item_codes)[i][j] == item_code[j]) {
                same_letter++;
            }
            
            if (same_letter == (ITEM_CODE_LENGTH-1)) {
                pos = i;
            }
        } 
    }
    
    int cost = (menu->item_cost_per_unit)[pos];
    return cost;
}

double get_order_subtotal (Order* order, Menu* menu) {
    double total_cost = 0;
    double cur_cost = 0;
    
    //Going through all the different items
    for (int i=0; i<(order->num_items); i++) {
        cur_cost = get_item_cost((order->item_codes)[i], menu);
        total_cost += cur_cost * (order->item_quantities)[i];
    }
    
    return total_cost;
}

double get_order_total (Order* order, Menu* menu) {
    double before_tax = get_order_subtotal(order, menu);
    double after_tax = before_tax * (1+(TAX_RATE/100));
    return after_tax;
}

int get_num_completed_orders (Restaurant* restaurant) {
    return restaurant->num_completed_orders;
}

int get_num_pending_orders (Restaurant* restaurant) {
    return restaurant->num_pending_orders;
}

// clear all memory associated with *order, call free(*order) and set it to NULL
void clear_order (Order** order) {
    Order* order_p = *order;
    
    for (int i=0; i<(order_p->num_items); i++) {
        free((order_p->item_codes)[i]);
    }
  
    free(order_p->item_codes);
    free(order_p->item_quantities);
  
    free(order_p);
    *order = NULL;
}

//Clear the memory assoicated with *menu --> call free(*menu) and set to Null
//fix the memory issues later
void clear_menu (Menu** menu) {
    Menu* menu_p = *menu;
  
    for (int i=0; i<(menu_p->num_items); i++) {
        free((menu_p->item_codes)[i]);
        free((menu_p->item_names)[i]);
    }
    free((menu_p->item_codes)[num_items]);
    free((menu_p->item_names)[num_items]);
    free(menu_p->item_codes);
    free(menu_p->item_names);

    free(menu_p->item_cost_per_unit);
    
    free(menu_p);
    *menu = NULL;
}

// clear all memory associated with *restaurant (all queuenods and orders included) --> free *restraurant and set to NULL
void close_restaurant (Restaurant** restaurant) {
    Restaurant* rest_p = *restaurant;
    clear_menu(&(rest_p->menu));
    QueueNode* new_head = rest_p->pending_orders->head;
    QueueNode* old_head = rest_p->pending_orders->head;
    
    //Clearing pending orders
    //Going through linked list of order queue
    while (new_head != NULL) {
        old_head = new_head;
        new_head = new_head->next;
        old_head->next = NULL;
      
        clear_order(&(old_head->order));
        free(old_head);
    }
    
    free(rest_p);
    *restaurant = NULL;
}


void print_menu(Menu* menu){
	fprintf(stdout, "--- Menu ---\n");
	for (int i = 0; i < menu->num_items; i++){
		fprintf(stdout, "(%s) %s: %.2f\n", 
			menu->item_codes[i], 
			menu->item_names[i], 
			menu->item_cost_per_unit[i]	
		);
	}
}


void print_order(Order* order){
	for (int i = 0; i < order->num_items; i++){
		fprintf(
			stdout, 
			"%d x (%s)\n", 
			order->item_quantities[i], 
			order->item_codes[i]
		);
	}
}


void print_receipt(Order* order, Menu* menu){
	for (int i = 0; i < order->num_items; i++){
		double item_cost = get_item_cost(order->item_codes[i], menu);
		fprintf(
			stdout, 
			"%d x (%s)\n @$%.2f ea \t %.2f\n", 
			order->item_quantities[i],
			order->item_codes[i], 
			item_cost,
			item_cost * order->item_quantities[i]
		);
	}
	double order_subtotal = get_order_subtotal(order, menu);
	double order_total = get_order_total(order, menu);
	
	fprintf(stdout, "Subtotal: \t %.2f\n", order_subtotal);
	fprintf(stdout, "               -------\n");
	fprintf(stdout, "Tax %d%%: \t$%.2f\n", TAX_RATE, order_total);
	fprintf(stdout, "              ========\n");
}
