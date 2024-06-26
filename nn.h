/* This one-file library is heavily inspired by Tsoding's nn.h .
you can find the source code in:
https://github.com/tsoding/nn.h .
featured in this video of his:
https://youtu.be/PGSba51aRYU?list=PLpM-Dvs8t0VZPZKggcql-MmjaBdZKeDMw .*/


#ifndef NN_H_
#define NN_H_

#include <stddef.h>
#include <stdio.h>
#include <math.h>

#ifndef NN_MALLOC
#include <stdlib.h>
#define NN_MALLOC malloc
#endif //NN_MALLOC

#ifndef NN_ASSERT
#include <assert.h>
#define NN_ASSERT assert
#endif //NN_ASSERT

/* MATRIXES */
typedef struct {
    size_t rows;
    size_t cols;
    size_t stride;
    float *elements;
    
} Mat;

#define MAT_AT(m, i, j) (m).elements[(i)*(m).stride + (j)]
#define MAT_PRINT(m) mat_print((m), #m, 0)

float rand_float(void);
Mat mat_alloc(size_t rows, size_t cols);
void mat_fill(Mat m, float x);
void mat_rand(Mat m, float low, float high);
void mat_dot(Mat dst, Mat a, Mat b);
void mat_sum(Mat dst, Mat a);
void mat_mult(Mat a, size_t factor);
void mat_print(Mat m, const char *name, size_t padding);
void mat_identity_mat(Mat m);
Mat mat_row(Mat m, size_t row);

float sigmoidf(float x);
void mat_sig(Mat m);
void mat_copy(Mat dst, Mat src);

/* NN */

#define ARRAY_LEN(xs) sizeof((xs))/sizeof((xs)[0])
#define NN_PRINT(nn) nn_print((nn), #nn)
#define NN_INPUT(nn) (nn).as[0]
#define NN_OUTPUT(nn) (nn).as[(nn).count]

typedef struct {
    size_t count;
    Mat *ws;
    Mat *bs;
    Mat *as; /* The amount of activations is count+1 */    
} NN;

NN nn_alloc(size_t *arch, size_t arch_count);
void nn_fill(NN nn, float x);
void nn_print(NN nn, const char *name);
void nn_rand(NN nn, float low, float high);
void nn_forward(NN nn);
float nn_cost(NN nn, Mat ti, Mat to);
void nn_finite_diff(NN nn, NN g, float eps, Mat ti, Mat to);
void nn_backprop(NN nn, NN g, Mat ti, Mat to);
void nn_learn(NN nn, NN g, float rate); 

#endif // NN_H_

#ifdef NN_IMPLEMENTATION

float rand_float(void)
{
    return (float) rand() / (float) RAND_MAX;
}

Mat mat_alloc(size_t rows, size_t cols)
{
    Mat m;
    m.rows = rows;
    m.cols = cols;
    m.stride = cols;
    m.elements = (float*)NN_MALLOC(sizeof(*m.elements)*rows*cols);
    NN_ASSERT(m.elements != NULL);
    return m;    
}

void mat_fill(Mat m, float x)
{
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            MAT_AT(m, i, j) = x;
        }
    }
}

void mat_rand(Mat m, float low, float high)
{
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            MAT_AT(m, i, j) = rand_float()*(high - low) + low;
        }
    }
}

void mat_dot(Mat dst, Mat a, Mat b)
{
    NN_ASSERT(a.cols == b.rows);
    size_t n = a.cols;
    NN_ASSERT(a.rows == dst.rows);
    NN_ASSERT(b.cols == dst.cols);
    mat_fill(dst, 0);

    for (size_t i = 0; i < dst.rows; i++) {
        for (size_t j = 0; j < dst.cols; j++) {
            for (size_t k = 0; k < n; k++) {
                MAT_AT(dst, i, j) += MAT_AT(a, i, k)*MAT_AT(b, k, j);
            }
        }
    }

}

void mat_sum(Mat dst, Mat a)
{
    NN_ASSERT(dst.rows == a.rows);
    NN_ASSERT(dst.cols == a.cols);
    for (size_t i = 0; i < dst.rows; ++i) {
        for (size_t j = 0; j < dst.cols; ++j) {
            MAT_AT(dst, i, j) += MAT_AT(a, i, j);
        }
    }
}

void mat_mult(Mat m, size_t factor)
{
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            MAT_AT(m, i, j) = MAT_AT(m, i, j) * factor;
        }
    }
}

void mat_print(Mat m, const char *name, size_t padding)
{
    printf("%*s%s = [\n", (int) padding, "", name);
    for (size_t i = 0; i < m.rows; ++i) {
        printf("%*s    ", (int) padding, "");
        for (size_t j = 0; j < m.cols; ++j) {
            printf("%f ", MAT_AT(m, i, j));
        }
        printf("\n");
    }
    printf("%*s]\n", (int) padding, "");
}

/* make matrix into the identity matrix*/
void mat_identity_mat(Mat m)
{
    NN_ASSERT(m.cols == m.rows);
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            if (i == j) {
                MAT_AT(m, i, j) = 1;
            }
            else {
                MAT_AT(m, i, j) = 0;
            }
        }
    }
}

Mat mat_row(Mat m, size_t row)
{
    return (Mat){
        .rows = 1,
        .cols = m.cols,
        .stride = m.stride,
        .elements = &MAT_AT(m, row, 0)
    };
}

float sigmoidf(float x)
{
    return 1.f / (1.f + expf(-x));
}

void mat_sig(Mat m)
{
    for (size_t i = 0; i < m.rows; ++i) {
        for (size_t j = 0; j < m.cols; ++j) {
            MAT_AT(m, i, j) = sigmoidf(MAT_AT(m, i, j));
        }
    }
    
}

void mat_copy(Mat dst, Mat src)
{
    NN_ASSERT(dst.cols == src.cols);
    NN_ASSERT(dst.rows == src.rows);

    for (size_t i = 0; i < dst.rows; ++i) {
        for (size_t j = 0; j < dst.cols; ++j) {
            MAT_AT(dst, i, j) = MAT_AT(src, i, j);
        }
    }
    
}

NN nn_alloc(size_t *arch, size_t arch_count)
{
    NN_ASSERT(arch_count > 0);

    NN nn;
    nn.count = arch_count - 1;

    nn.ws = NN_MALLOC(sizeof(*nn.ws)*nn.count);
    NN_ASSERT(nn.ws != NULL);
    nn.bs = NN_MALLOC(sizeof(*nn.bs)*nn.count); 
    NN_ASSERT(nn.bs != NULL);
    nn.as = NN_MALLOC(sizeof(*nn.as)*(nn.count + 1)); 
    NN_ASSERT(nn.as != NULL);

    nn.as[0] = mat_alloc(1, arch[0]);
    for (size_t i = 1; i < arch_count; ++i) {
        nn.ws[i-1] = mat_alloc(nn.as[i-1].cols, arch[i]);
        nn.bs[i-1] = mat_alloc(1, arch[i]);
        nn.as[i]   = mat_alloc(1, arch[i]);
    }
    
    return nn;
}

void nn_fill(NN nn, float x)
{
    for (size_t i = 0; i < nn.count; ++i) {
        mat_fill(nn.ws[i], x);
        mat_fill(nn.bs[i], x);
        mat_fill(nn.as[i], x);
    }

    mat_fill(nn.as[nn.count], x);
}

void nn_print(NN nn, const char *name)
{
    char buf[256];
    printf("%s = [\n", name);
    for (size_t i = 0; i < nn.count; ++i) {
        snprintf(buf, sizeof(buf), "ws%zu", i);
        mat_print(nn.ws[i], buf, 4);
        snprintf(buf, sizeof(buf), "bs%zu", i);
        mat_print(nn.bs[i], buf, 4);

    }
}

void nn_rand(NN nn, float low, float high)
{
    for (size_t i = 0; i < nn.count; ++i) {
        mat_rand(nn.ws[i], low, high);
        mat_rand(nn.bs[i], low, high);
    }
}

void nn_forward(NN nn)
{
    for (size_t i = 0; i < nn.count; i++) {
        mat_dot(nn.as[i+1], nn.as[i], nn.ws[i]);
        mat_sum(nn.as[i+1], nn.bs[i]);
        mat_sig(nn.as[i+1]);
    }
}

float nn_cost(NN nn, Mat ti, Mat to)
{
    NN_ASSERT(ti.rows == to.rows);
    NN_ASSERT(to.cols == NN_OUTPUT(nn).cols);
    size_t n = ti.rows, q = to.cols;
    float d, c = 0;

    for (size_t i = 0; i < n; ++i) {
        Mat input = mat_row(ti, i);
        Mat output = mat_row(to, i);

        mat_copy(NN_INPUT(nn), input);
        nn_forward(nn);
        
        for (size_t j = 0; j < q; j++) {
            d =  MAT_AT(NN_OUTPUT(nn), 0, j) - MAT_AT(output, 0, j);
            c += d*d;
        }
    }
    return c/n;
}

void nn_finite_diff(NN nn, NN g, float eps, Mat ti, Mat to)
{
    float saved, c = nn_cost(nn, ti, to);

    for (size_t i = 0; i < nn.count; ++i) {
        for (size_t j = 0; j < nn.ws[i].rows; ++j) {
            for (size_t k = 0; k < nn.ws[i].cols; ++k) {
                saved = MAT_AT(nn.ws[i], j, k);
                MAT_AT(nn.ws[i], j, k) += eps;
                MAT_AT(g.ws[i], j, k) = (nn_cost(nn, ti, to) - c)/eps;
                MAT_AT(nn.ws[i], j, k) = saved;
            }
        }

        for (size_t j = 0; j < nn.bs[i].rows; ++j) {
            for (size_t k = 0; k < nn.bs[i].cols; ++k) {
                saved = MAT_AT(nn.bs[i], j, k);
                MAT_AT(nn.bs[i], j, k) += eps;
                MAT_AT(g.bs[i], j, k) = (nn_cost(nn, ti, to) - c)/eps;
                MAT_AT(nn.bs[i], j, k) = saved;
            }
        }
    } 
}

void nn_backprop(NN nn, NN g, Mat ti, Mat to)
{
    NN_ASSERT(ti.rows == to.rows);
    NN_ASSERT(NN_OUTPUT(nn).cols == to.cols);

    size_t n = ti.rows;
    float a, da, pa, w;

    nn_fill(g, 0);

/*  i - current sample
    j - current activation
    l - current layer
    k - previous activation */

    for (size_t i = 0; i < n; i++) {
        mat_copy(NN_INPUT(nn), mat_row(ti, i));
        nn_forward(nn);
        
        for (size_t j = 0; j <= nn.count; ++j) {
            mat_fill(g.as[j], 0);
        }
        
        for (size_t j = 0; j < to.cols; ++j) {
            MAT_AT(NN_OUTPUT(g), 0, j) = MAT_AT(NN_OUTPUT(nn), 0, j) - MAT_AT(to, i, j);
        }

        for (size_t l = nn.count; l > 0; --l) {
            for (size_t j = 0; j < nn.as[l].cols; j++) {
            /*  j - weight matrix col
                k - weight matrix row   */
                a = MAT_AT(nn.as[l], 0, j);
                da = MAT_AT(g.as[l], 0, j);
                MAT_AT(g.bs[l-1], 0, j) += 2*da*a*(1-a);
                for (size_t k = 0; k < nn.as[l-1].cols; k++) {
                    pa = MAT_AT(nn.as[l-1], 0, k);
                    w = MAT_AT(nn.ws[l-1], k, j);
                    MAT_AT(g.ws[l-1], k, j) += 2*da*a*(1-a)*pa;
                    MAT_AT(g.as[l-1], 0, k) += 2*da*a*(1-a)*w;
                }
            }
        }
    }

    for (size_t i = 0; i < g.count; ++i) {
        for (size_t j = 0; j < g.ws[i].rows; j++) {
            for (size_t k = 0; k < g.ws[i].cols; k++) {
                MAT_AT(g.ws[i], j, k) /= n;
            }
        }
        
        for (size_t j = 0; j < g.bs[i].rows; j++) {
            for (size_t k = 0; k < g.bs[i].cols; k++) {
                MAT_AT(g.bs[i], j, k) /= n;
            }
        }
    }
}

void nn_learn(NN nn, NN g, float rate) 
{
    for (size_t i = 0; i < nn.count; ++i) {
        for (size_t j = 0; j < nn.ws[i].rows; ++j) {
            for (size_t k = 0; k < nn.ws[i].cols; ++k) {
                MAT_AT(nn.ws[i], j, k) -= rate * MAT_AT(g.ws[i], j, k);
            }
        }

        for (size_t j = 0; j < nn.bs[i].rows; ++j) {
            for (size_t k = 0; k < nn.bs[i].cols; ++k) {
                MAT_AT(nn.bs[i], j, k) -= rate * MAT_AT(g.bs[i], j, k);
            }
        }
    } 
}

#endif // NN_IMPLEMENTATION
