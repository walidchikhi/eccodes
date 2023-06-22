/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/*
   This is used by make_class.pl

   START_CLASS_DEF
   CLASS      = accessor
   SUPER      = grib_accessor_class_data_shsimple_packing
   IMPLEMENTS = init
   IMPLEMENTS = unpack_double
   IMPLEMENTS = pack_double
   IMPLEMENTS = value_count
   MEMBERS=const char*  numberOfValues
   MEMBERS=const char*  numberOfDataPoints
   END_CLASS_DEF
 */

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int pack_double(grib_accessor*, const double* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void init(grib_accessor*, const long, grib_arguments*);
//static void init_class(grib_accessor_class*);

typedef struct grib_accessor_data_g2shsimple_packing
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in data_shsimple_packing */
    const char*  coded_values;
    const char*  real_part;
    int dirty;
    /* Members defined in data_g2shsimple_packing */
    const char*  numberOfValues;
    const char*  numberOfDataPoints;
} grib_accessor_data_g2shsimple_packing;

extern grib_accessor_class* grib_accessor_class_data_shsimple_packing;

static grib_accessor_class _grib_accessor_class_data_g2shsimple_packing = {
    &grib_accessor_class_data_shsimple_packing,                      /* super */
    "data_g2shsimple_packing",                      /* name */
    sizeof(grib_accessor_data_g2shsimple_packing),  /* size */
    0,                           /* inited */
    0,                           /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    0,                    /* destroy */
    0,                       /* dump */
    0,                /* next_offset */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    0,            /* get native type */
    0,                /* get sub_section */
    0,               /* pack_missing */
    0,                 /* is_missing */
    0,                  /* pack_long */
    0,                /* unpack_long */
    &pack_double,                /* pack_double */
    0,                 /* pack_float */
    &unpack_double,              /* unpack_double */
    0,               /* unpack_float */
    0,                /* pack_string */
    0,              /* unpack_string */
    0,          /* pack_string_array */
    0,        /* unpack_string_array */
    0,                 /* pack_bytes */
    0,               /* unpack_bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value (double) */
    0,       /* unpack only ith value (float) */
    0,  /* unpack a given set of elements (double) */
    0,   /* unpack a given set of elements (float) */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_data_g2shsimple_packing = &_grib_accessor_class_data_g2shsimple_packing;


//static void init_class(grib_accessor_class* c)
//{
// INIT
//}

/* END_CLASS_IMP */

static void init(grib_accessor* a, const long v, grib_arguments* args)
{
    grib_accessor_data_g2shsimple_packing* self = (grib_accessor_data_g2shsimple_packing*)a;

    self->numberOfValues     = grib_arguments_get_name(grib_handle_of_accessor(a), args, 2);
    self->numberOfDataPoints = grib_arguments_get_name(grib_handle_of_accessor(a), args, 3);
    a->flags |= GRIB_ACCESSOR_FLAG_DATA;
}

static int value_count(grib_accessor* a, long* len)
{
    grib_accessor_data_g2shsimple_packing* self = (grib_accessor_data_g2shsimple_packing*)a;
    *len                                        = 0;
    return grib_get_long(grib_handle_of_accessor(a), self->numberOfValues, len);
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_data_g2shsimple_packing* self = (grib_accessor_data_g2shsimple_packing*)a;
    int err                                     = GRIB_SUCCESS;

    size_t n_vals = 0;

    if ((err = grib_get_size(grib_handle_of_accessor(a), self->coded_values, &n_vals)) != GRIB_SUCCESS)
        return err;

    self->dirty = 0;

    /* n_vals = coded_n_vals+1; */

    if (*len < n_vals) {
        *len = n_vals;
        return GRIB_ARRAY_TOO_SMALL;
    }

    if ((err = grib_get_double_internal(grib_handle_of_accessor(a), self->real_part, val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_get_double_array_internal(grib_handle_of_accessor(a), self->coded_values, val, &n_vals)) != GRIB_SUCCESS)
        return err;

    *len = n_vals;

    return err;
}

static int pack_double(grib_accessor* a, const double* val, size_t* len)
{
    grib_accessor_data_g2shsimple_packing* self = (grib_accessor_data_g2shsimple_packing*)a;
    int err                                     = GRIB_SUCCESS;

    size_t coded_n_vals = *len - 1;
    size_t n_vals       = *len;

    if (*len == 0)
        return GRIB_NO_VALUES;

    self->dirty = 1;

    if ((err = grib_set_double_internal(grib_handle_of_accessor(a), self->real_part, *val)) != GRIB_SUCCESS)
        return err;

    val++;

    if ((err = grib_set_double_array_internal(grib_handle_of_accessor(a), self->coded_values, val, coded_n_vals)) != GRIB_SUCCESS)
        return err;

    *len = n_vals;

    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), self->numberOfValues, (long)n_vals)) != GRIB_SUCCESS)
        return err;
    if ((err = grib_set_long_internal(grib_handle_of_accessor(a), self->numberOfDataPoints, (long)n_vals)) != GRIB_SUCCESS)
        return err;

    return err;
}
