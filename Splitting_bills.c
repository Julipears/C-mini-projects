#include <math.h>
#include <stdio.h>
#include <string.h>

double split_bill(double base_amount, double tax_rate, double tip_rate, int num_people);

double adjust_price(double original_price);

int sandy_eats(char menu_item[]);

void imagine_fish(char thing []);

//Part 1
double split_bill(double base_amount, double tax_rate, double tip_rate , int num_people) 
{
    double final_bill = 0;
    double tax = base_amount * tax_rate;
    double tip = (base_amount + tax) * tip_rate;
    double full_amount = base_amount + tax + tip;
    final_bill = (base_amount + tax + tip) / num_people;
    
    // rounding up
    double rounded_final = round(final_bill*100)/100;
    while (rounded_final*num_people < full_amount)
    {
        final_bill += 0.01;
        rounded_final = round(final_bill*100)/100;
    }
    
    return final_bill;
}


//Part 2
double adjust_price(double original_price)
{
    double exp = pow(original_price, 0.5);
    double final_price = 10*exp;
    return final_price;
}


//Part 3

//T-T it doesn't work :((((
int sandy_eats(char menu_item[])
{
    if (strstr(menu_item, "j") != 0||strstr(menu_item, "k") != 0||strstr(menu_item, "l") != 0||strstr(menu_item, "J") != 0||strstr(menu_item, "K") != 0||strstr(menu_item, "L") != 0)
    {
        return 0;
    }
    
    if (strstr(menu_item, "fish") != 0) // Try to write NULL instead of 0, better form
    {
        return 0;
    }
    
    int count = 0;
    int letters = 0;
    
    char space[] = " ";
    while (count < strlen(menu_item))
    {
        if (menu_item[count] == space[0])
        {
            letters = letters - 1;
        }
        
        letters ++;
        count ++;
        
    }
    
    if (letters %2 == 1)
    {
        return 0;
    }
    
    return 1;
}


//Part 4
void imagine_fish(char thing [])
{
    strcat(thing, "fish");
}
