## Frequent Itemsets Mining using FP Growth Algorithm

### Database filename and minimum support percentage are passed as command line arguments.

```
g++ FPGrowth.cpp

a.exe database_file_name min_support_percentage

```


### *Note: Database file format- integer labels to represent items for each transaction separated by white spaces. Each transaction starts in a new line.*

### File named *output.txt* is produced containing all frequent itemsets along with their support counts. 