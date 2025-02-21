# IPC Clothing Shop Simulator
This is a three process application that simulates a clothing shop interacting together using shared memory. The three process are:

1 - `Shop`: initializes the stock then periodically prints out the number of each item in stock and its price.

2 - `Customer`: purchases items from the shop.

3 - `Inventory Manager`: manages the shop's inventory by restocking when an item is low or can also be used to edit the prices.

## Compilation
To compile the program files use:
```
gcc shop.c -o shop
gcc customer.c -o customer
gcc inv.c -o inv
```

## Examples
### Shop
```
omar@Ubuntu22:~/Desktop/IPC$ ./shop

Current Stock:
Count   Price   Item
5       43.00   Jeans
1       34.00   T-Shirt
2       94.00   Jacket

Current Stock:
Count   Price   Item
5       43.00   Jeans
50      34.00   T-Shirt
2       37.50   Jacket
```

### Customer
```
mar@Ubuntu22:~/Desktop/IPC$ ./customer 

Welcome, select the number of the item you want:
No.     Price   Name    Status
0       43.00   Jeans   In Stock
1       34.00   T-Shirt In Stock
2       94.00   Jacket  In Stock

Select item: 1
Purchase Successful!
```

### Inventory Manager
```
omar@Ubuntu22:~/Desktop/IPC$ ./inv

Current Stock:
No.     Count   Price   Name
0       5       43.00   Jeans
1       0       34.00   T-Shirt
2       2       94.00   Jacket

Enter R to restock or P to update prices: R
Select item: 1
Restock amount: 50
T-Shirt restocked!

Current Stock:
No.     Count   Price   Name
0       5       43.00   Jeans
1       50      34.00   T-Shirt
2       2       94.00   Jacket

Enter R to restock or P to update prices: P
Select item: 2
New Price: 37.50
Jacket price changed!
```
