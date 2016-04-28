// START const
#define NAME (VAL)
// END const

#include <string.h> // START header
#include <cstdlib>
#include <cstdint>
#include <stddef.h>
#include <stdio.h>

using FdType = int;
typedef uint64_t opaque;


struct native_handle
{
    int version;        /* sizeof(native_handle_t) */
    int numFds;         /* number of file-descriptors at &data[0] */
    int numInts;        /* number of ints at &data[numFds] */
    int data[0];        /* numFds + numInts ints */
};

typedef int32_t ref;
typedef int32_t fd;

template<typename T>
struct vec {
  size_t count;
  T *ptr;
};

struct HidlString {
  ptrdiff_t n_bytes;
  char *buffer;
};




struct FunStruct {
  alignas(8) FunStruct *self_ptr;
  alignas(8) size_t n_fds;
};

#define ALIGNMENT (8)
#define ALIGNED(size) (((size_t)(size) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))
#define PALIGNED(ptr) ((char *)ALIGNED((size_t)(ptr)))

template<typename T>
size_t bytes_needed(T var)
{
  (void)var; return sizeof(T);
}
size_t bytes_needed(HidlString& var)
{
  ptrdiff_t buf_len = (var.n_bytes == -1) ? strlen(var.buffer) : var.n_bytes;
  return sizeof(HidlString) + ALIGNED(buf_len);
}
size_t bytes_needed(native_handle &var)
{
  (void)var;
  return 5;
}

template<typename T>
size_t fds_needed(T &var)
{
  (void)var;
  return 0;
}
size_t fds_needed(native_handle &var)
{
  return var.numFds;
}
size_t fds_needed(ref &var)
{
  (void)var;
  return 1;
}

void extract_fds(FdType **ppFds, ref var)
{
  *(*ppFds++) = (FdType)var;
}

void extract_fds(FdType **ppFds, native_handle& var)
{
  for (int i = 0; i < var.numFds; i++) {
    *(*ppFds++) = (FdType)var.data[i];
  }
}
void replace_fds(FdType **ppFds, native_handle* var)
{
  for (int i = 0; i < var->numFds; i++) {
    *(*ppFds++) = (FdType)var->data[i];
  }
}

size_t fix_pointers(native_handle *var, ptrdiff_t fix)
{
  (void)var;
  (void)fix;
  return 5; // TODO
}

void fill_data(char **ppData, native_handle& var)
{
  (void)ppData;
  (void)var; // TODO
}
// END header


struct C {
  ref r;
};
size_t bytes_needed(C &var)
{
  (void)var;
  return sizeof(C);
}
size_t fds_needed(C &var)
{
  (void)var;
  return 1;
}
void extract_fds(FdType **ppFds, C& var)
{
  *(*ppFds++) = var.r;
}
void replace_fds(FdType **ppFds, C& var)
{
  var.r = *(*ppFds++);
}

struct B {
  HidlString s;
};
size_t bytes_needed(B &var)
{
  (void)var;
  return 5;
}
size_t fds_needed(B &var)
{
  (void)var;
  return 5;
}
void extract_fds(FdType **ppFds, B& var)
{
  (void)ppFds; (void)var;
}
void replace_fds(FdType **ppFds, B& var)
{
  (void)ppFds;(void)var;
}

#define array_size 3
#define array_base C
#define vec_name b
#define array_name c

// START struct_code
struct struct_type {
  // ALL field_declarations
  vec<B> vec_name; // SKIP
  array_base array_name[array_size]; // SKIP
};
typedef struct_type A; // SKIP

size_t bytes_needed(struct_type &var)
{
  size_t sum = 0, temp_size, i;
  sum += sizeof(struct_type);
  // START b_n_fields
  temp_size = 0;    // START b_n_vec
  for (i = 0; i < var.vec_name.count; i++) {
    temp_size += bytes_needed(var.vec_name.ptr[i]);
  }
  sum += ALIGNED(temp_size); // END b_n_vec
  temp_size = 0; // START b_n_array
  for (i = 0; i < array_size; i++) {
    temp_size += ALIGNED(bytes_needed(var.array_name[i]) - sizeof(array_base));
  }
  sum += temp_size; // END b_n_array
  // END b_n_fields
  return sum;
}

size_t fds_needed(struct_type& var)
{
  size_t sum = 0;
  size_t i;
  // START fd_n_fields
  for (i = 0; i < var.vec_name.count; i++) { // START fd_n_vec
    sum += fds_needed(var.vec_name.ptr[i]);
  } // END fd_n_vec
  for (i = 0; i < array_size; i++) { // START fd_n_array
    sum += fds_needed(var.array_name[i]);
  } // END fd_n_array
  // END fd_n_fields
  return sum;
}

void extract_fds(FdType **ppFds, struct_type& var)
{
  size_t i;
  // START xfd_fields
  for (i = 0; i < var.vec_name.count; i++) { // START xfd_vec
    extract_fds(ppFds, var.vec_name.ptr[i]);
  } // END xfd_vec
  for (i = 0; i < array_size; i++) { // START xfd_array
    extract_fds(ppFds, var.array_name[i]);
  } // END xfd_array
  // END xfd_fields
}

void replace_fds(FdType **ppFds, struct_type* var)
{
  size_t i;
  // START rfd_fields
  for (i = 0; i < var->vec_name.count; i++) { // START rfd_vec
    replace_fds(ppFds, var->vec_name.ptr[i]);
  } // END rfd_vec
  for (i = 0; i < array_size; i++) { // START rfd_array
    replace_fds(ppFds, var->array_name[i]);
  } // END rfd_array
  // END rfd_fields
}

void fill_data(char **ppData, struct_type& var)
{
  (void)ppData;
  (void)var; // TODO
}

size_t fix_pointers(struct_type *var, ptrdiff_t fix)
{
  (void)var;
  (void)fix;
  return 5; // TODO
}
//END struct_code

void dispatch_to_fun(char *wirebuf);


void kernel_trap_for_funcall(char* buf, size_t len)
{
  char *raw_buf = (char *)malloc(len + ALIGNMENT);
  char *new_buf = PALIGNED(raw_buf);
  memcpy(new_buf, buf, len);
  printf("Pointers should be between %p and %p\n", new_buf, new_buf+len);
  FunStruct *pFS = (FunStruct *)new_buf;
  pFS->self_ptr = pFS;
  int *pFds = (FdType *)(new_buf + ALIGNED(sizeof(FunStruct)));
  for (size_t i = 0; i < pFS->n_fds; i++) {
    pFds[i] *= 3;
  }

  dispatch_to_fun(new_buf);
  (void)len;
}

#define type A
void fun( // START proxy
    type& arg // ALL param
    , native_handle &h // SKIP
)
{
  size_t bytes_sum = sizeof(FunStruct);
  bytes_sum += ALIGNED(bytes_needed(arg)); // ALL sum_bytes
  bytes_sum += ALIGNED(bytes_needed(h)); // SKIP
  size_t fds_sum = 0;
  fds_sum += fds_needed(arg); // ALL sum_fds
  fds_sum += fds_needed(h); // SKIP
  size_t buf_size = ALIGNED(bytes_sum) +
      ALIGNED(fds_sum * sizeof(FdType));
  char *raw_buf = (char *)malloc(buf_size + ALIGNMENT);
  char *wire_buf = PALIGNED(raw_buf);
  FunStruct *pFS = (FunStruct *)wire_buf;
  pFS->self_ptr = pFS;
  int *pFds = (FdType *)(wire_buf + ALIGNED(sizeof(FunStruct)));
  char *pData = wire_buf + ALIGNED(sizeof(FunStruct)) +
      ALIGNED(fds_sum * sizeof(FdType));
  extract_fds(&pFds, arg); // ALL get_fds
  extract_fds(&pFds, h); // SKIP
  fill_data(&pData, arg);  // START move_data
  pData = PALIGNED(pData);  // END move_data
  fill_data(&pData, h);  // SKIP
  pData = PALIGNED(pData); // SKIP
  kernel_trap_for_funcall(wire_buf, buf_size);
  free(raw_buf);
} // END proxy

void targetFun(A& a, native_handle& h);

void dispatch_to_fun(char *wire_buf) {
  // Assumes wire_buf is aligned to ALIGNMENT!
  FunStruct *pFS = (FunStruct *)wire_buf;
  char *pData;
  ptrdiff_t ptr_fixup = (char *)pFS - (char *)pFS->self_ptr;
  fd *pFds = (fd *)(wire_buf + ALIGNED(sizeof(FunStruct)));
  pData = PALIGNED(wire_buf + (pFS->n_fds * sizeof(FdType)));
  A *a_ptr = (A *)pData;
  pData += fix_pointers(a_ptr, ptr_fixup);
  pData = PALIGNED(pData);
  native_handle *h_ptr = (native_handle *)pData;
  pData += fix_pointers(h_ptr, ptr_fixup);
  pData = PALIGNED(pData);
  replace_fds(&pFds, a_ptr);
  replace_fds(&pFds, h_ptr);
  targetFun(*a_ptr, *h_ptr);
}

void targetFun(A& a, native_handle& h)
{
  printf("Target acquired... h[0] is %d\n", h.data[0]);
  printf("a.c values are %d, %d, %d\n", a.c[0].r, a.c[1].r, a.c[2].r);
  printf("a.b strings are %s and %s\n", a.b.ptr[0].s.buffer,
         a.b.ptr[1].s.buffer);
  printf("Addresses are %p, %p, %p, %p\n", &a, &h, a.b.ptr,
         a.b.ptr[0].s.buffer);
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  printf("Hello world\n");
  int h_space[6];
  native_handle *h = (native_handle *)&h_space;
  h->numFds = 1;
  int *pData = h->data;
  pData[0] = 2;
  A a;
  B b[2];
  a.b.ptr = b;
  a.b.count = 2;
  a.c[0].r = 5;
  a.c[1].r = 42;
  a.c[2].r = 13;
  b[0].s.n_bytes = -1;
  b[0].s.buffer = (char *)"Kilroy was here"; // this is bad form
  b[1].s.n_bytes = -1;
  b[1].s.buffer = (char *)"Where's Waldo?";
  fun(a, *h);
}
