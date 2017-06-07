#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

#include <omp.h>

int v = 7;
int b = 7;
int k = 3;
int r = 3;
int lambda = 1;


void print_block(int block[k]) {

   for (int i = 0; i < k; i++) {
      printf("%d ", block[i]);
   }
   printf("\n");
}

void print_matrix(int matrix[b][k]) {

   for (int i = 0; i < b; i++) {
      print_block(matrix[i]);
   }
   printf("\n");
}

// Returns the number of times an object is in a block
int object_in_block(int block[k], int object) {

   int count = 0;
   for (int i = 0; i < k; i++) {
      if (block[i] == object) {
	 count++;
      }
   }
   return count;
}

int number_times_object_is_used(int matrix[b][k], int object) {

   int sum = 0;
   for (int i = 0; i < b; i++) {
      sum += object_in_block(matrix[i], object);
   }
   return sum;
}


bool pairs_of_objects_appear_lambda_times(int matrix[b][k]) {
   // make the pairs of objects, order doesn't matter so start b at a + 1
   for (int object_a = 1; object_a < v; object_a++) {
      for (int object_b = object_a + 1; object_b <= v; object_b++) {

	 // number of blocks the pair is in
	 int block_count = 0;
	 for (int block_index = 0; block_index < b; block_index++) {

	    int a = object_in_block(matrix[block_index], object_a);
	    int b = object_in_block(matrix[block_index], object_b);
	    if (a == 1 && b == 1) {
	       block_count++;
	    }
	 }
	 if (block_count != lambda) { return false; }
      }
   }
   return true;
}

bool every_object_appears_in_r_blocks(int matrix[b][k]) {

   for (int object = 1; object <= v; object++) {

      int blocks_object_is_in = 0;
      for (int block_index = 0; block_index < b; block_index++) {
      
	 int temp = object_in_block(matrix[block_index], object);
	 if (temp > 1) { return false; }
	 else if (temp == 1) { blocks_object_is_in++; }
      }
      if (blocks_object_is_in != r) { return false; }
   }
   return true;
}

bool satisfies_requirements(int matrix[b][k]) {

   // working
   return pairs_of_objects_appear_lambda_times(matrix)
      && every_object_appears_in_r_blocks(matrix);
}

bool statisfies_partial_construction(int matrix[b][k]) {

   // plus one because the items are 1 indexed
   int item_counts[v + 1];

   for (int block = 0; block < b; block++) {

      for (int elem_index = 0; elem_index < k; elem_index++) {

	 // substitution to prevent memory reads
	 int item = matrix[block][elem_index];
	 item_counts[item]++;
	 //if (item_counts[item] > r || object_in_block(item) > 1) { return false; }
      }
   }
   return true;
}

void enumerate_matricies(int matrix[b][k], int row, int col) {

   // If the recursion has reached the final matrix postion
   if (row == b-1 && col == k-1) {
      #pragma omp parallel for
      for (int rand_num = 1;
	   rand_num <= v;
	   rand_num++) {

	 for (int i = 0; i < k; i++) {
	    if (rand_num == matrix[row][i]){
	       rand_num++;
	    }
	 }

	 matrix[row][col] = rand_num;
	 
	 //printf("matrix[%d][%d]=%d\n", row, col, matrix[row][col]);
	 if (satisfies_requirements(matrix)) {
	    print_matrix(matrix);
	    printf("Passed Requirements\n");
	 }

	 //print_matrix(matrix);
	 //sleep(1);
	 //printf("Failed Requirements\n");
      }
      // Bonk outta that recursion
      return;
   }

   int new_col;
   int new_row;
   #pragma omp parallel for
      for (int rand_num = 1; rand_num <= v; rand_num++) {

	 // Only add an object to the design if it's not in the block already
	 //   or if the allotted number of uses per object is already used
	 while (object_in_block(matrix[row], rand_num) != 0)
	    //number_times_object_is_used(matrix, rand_num) >= r)
	    {
	    
	       rand_num = ((rand_num + 1) % v); // Also don't add anything that's not an object
	    }
	 matrix[row][col] = rand_num;

	 // Increment the position in the matrix we want to iterate
	 new_col = col + 1;
	 new_row = row;
	 if (new_col >= k) {
	    new_row++;
	    new_col = 0;
	 }

	 enumerate_matricies(matrix, new_row, new_col);
      }
}

bool meets_specifications() {

   return (v*r) == (b*k) && ((v-1)*lambda) == (r*(k-1));
}

int main() {

   int design[7][3] = {
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0},
      {0, 0, 0}
   };

   // Just checks the primary property of block designs to ensure a design exists
   if (!meets_specifications()) { return EXIT_SUCCESS; }

   /*
     int correct[7][3] = {
     {1, 2, 4},
     {2, 3, 5},
     {3, 4, 6},
     {4, 5, 7},
     {1, 5, 6},
     {2, 6, 7},
     {1, 3, 7}
     };
     print_matrix(correct);
     printf("%d\n", satisfies_requirements(correct));
   //*/

   //return EXIT_SUCCESS;
   enumerate_matricies(design, 0, 0);
   
   return EXIT_SUCCESS;
}
