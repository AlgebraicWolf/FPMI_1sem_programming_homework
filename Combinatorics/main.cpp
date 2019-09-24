#include <stdio.h>
#include <assert.h>

unsigned long long C(unsigned long long n, unsigned long long k);

unsigned long long A(unsigned long long n, unsigned long long k);

int main() {
  FILE *input = fopen("input.txt", "r");
  FILE *output = fopen("output.txt", "w");

  assert(input);
  assert(output);
  unsigned long long n = 0;
  unsigned long long k = 0;
  unsigned long long result;
  char op = 'F';
  fscanf(input, "%c %llu %llu", &op, &n, &k);
  fclose(input);

  if (op == 'C') {
    result = C(n, k);
  } else if (op == 'A') {
    result = A(n, k);
  } else
    return -1;

  fprintf(output, "%llu", result);
  return 0;
}

unsigned long long C(unsigned long long n, unsigned long long k) {
  if (k > n) return 0;
  unsigned long long res = 1;

  for (unsigned long long i = 1; i <= k; i++) {
    res *= n - k + i;
    res /= i;
  }

  return res;
}

unsigned long long A(unsigned long long n, unsigned long long k) {
  if (k > n) return 0;
  unsigned long long res = 1;

  for (unsigned long long i = n - k + 1; i <= n; i++)
    res *= i;

  return res;
}