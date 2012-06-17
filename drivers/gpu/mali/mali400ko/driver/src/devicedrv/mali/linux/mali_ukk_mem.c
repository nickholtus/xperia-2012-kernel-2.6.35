/*
 * Copyright (C) 2010 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <linux/fs.h>       /* file system operations */
#include <asm/uaccess.h>    /* user space access */

#include "mali_ukk.h"
#include "mali_osk.h"
#include "mali_kernel_common.h"
#include "mali_kernel_session_manager.h"
#include "mali_ukk_wrappers.h"

int mem_init_wrapper(struct mali_session_data *session_data, _mali_uk_init_mem_s __user *uargs)
{
    _mali_uk_init_mem_s kargs;
    _mali_osk_errcode_t err;

    MALI_CHECK_NON_NULL(uargs, -EINVAL);

    kargs.ctx = session_data;
    err = _mali_ukk_init_mem(&kargs);
    if (_MALI_OSK_ERR_OK != err)
    {
        return map_errcode(err);
    }

    if (0 != put_user(kargs.mali_address_base, &uargs->mali_address_base)) goto mem_init_rollback;
    if (0 != put_user(kargs.memory_size, &uargs->memory_size)) goto mem_init_rollback;

    return 0;

mem_init_rollback:
	{
		_mali_uk_term_mem_s kargs;
		kargs.ctx = session_data;
		err = _mali_ukk_term_mem(&kargs);
		if (_MALI_OSK_ERR_OK != err)
		{
			MALI_DEBUG_PRINT(4, ("reverting _mali_ukk_init_mem, as a result of failing put_user(), failed\n"));
		}
	}
    return -EFAULT;
}

int mem_term_wrapper(struct mali_session_data *session_data, _mali_uk_term_mem_s __user *uargs)
{
    _mali_uk_term_mem_s kargs;
    _mali_osk_errcode_t err;

    MALI_CHECK_NON_NULL(uargs, -EINVAL);

    kargs.ctx = session_data;
    err = _mali_ukk_term_mem(&kargs);
    if (_MALI_OSK_ERR_OK != err)
    {
        return map_errcode(err);
    }

    return 0;
}

int mem_map_ext_wrapper(struct mali_session_data *session_data, _mali_uk_map_external_mem_s __user * argument)
{
	_mali_uk_map_external_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/* validate input */
	/* the session_data pointer was validated by caller */
    MALI_CHECK_NON_NULL( argument, -EINVAL);

	/* get call arguments from user space. copy_from_user returns how many bytes which where NOT copied */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_map_external_mem_s)) )
	{
		return -EFAULT;
	}

    uk_args.ctx = session_data;
	err_code = _mali_ukk_map_external_mem( &uk_args );

    if (0 != put_user(uk_args.cookie, &argument->cookie))
    {
        if (_MALI_OSK_ERR_OK == err_code)
        {
            /* Rollback */
           	_mali_uk_unmap_external_mem_s uk_args_unmap;

            uk_args_unmap.ctx = session_data;
            uk_args_unmap.cookie = uk_args.cookie;
            err_code = _mali_ukk_unmap_external_mem( &uk_args_unmap );
            if (_MALI_OSK_ERR_OK != err_code)
            {
                MALI_DEBUG_PRINT(4, ("reverting _mali_ukk_unmap_external_mem, as a result of failing put_user(), failed\n"));
            }
        }
        return -EFAULT;
    }

    /* Return the error that _mali_ukk_free_big_block produced */
	return map_errcode(err_code);
}

int mem_unmap_ext_wrapper(struct mali_session_data *session_data, _mali_uk_unmap_external_mem_s __user * argument)
{
	_mali_uk_unmap_external_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/* validate input */
	/* the session_data pointer was validated by caller */
    MALI_CHECK_NON_NULL( argument, -EINVAL);

	/* get call arguments from user space. copy_from_user returns how many bytes which where NOT copied */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_unmap_external_mem_s)) )
	{
		return -EFAULT;
	}

    uk_args.ctx = session_data;
	err_code = _mali_ukk_unmap_external_mem( &uk_args );

	/* Return the error that _mali_ukk_free_big_block produced */
	return map_errcode(err_code);
}

#if MALI_USE_UNIFIED_MEMORY_PROVIDER != 0
int mem_release_ump_wrapper(struct mali_session_data *session_data, _mali_uk_release_ump_mem_s __user * argument)
{
	_mali_uk_release_ump_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/* validate input */
	/* the session_data pointer was validated by caller */
    MALI_CHECK_NON_NULL( argument, -EINVAL);

	/* get call arguments from user space. copy_from_user returns how many bytes which where NOT copied */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_release_ump_mem_s)) )
	{
		return -EFAULT;
	}

    uk_args.ctx = session_data;
	err_code = _mali_ukk_release_ump_mem( &uk_args );

	/* Return the error that _mali_ukk_free_big_block produced */
	return map_errcode(err_code);
}

int mem_attach_ump_wrapper(struct mali_session_data *session_data, _mali_uk_attach_ump_mem_s __user * argument)
{
	_mali_uk_attach_ump_mem_s uk_args;
	_mali_osk_errcode_t err_code;

	/* validate input */
	/* the session_data pointer was validated by caller */
    MALI_CHECK_NON_NULL( argument, -EINVAL);

	/* get call arguments from user space. copy_from_user returns how many bytes which where NOT copied */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_attach_ump_mem_s)) )
	{
		return -EFAULT;
	}

    uk_args.ctx = session_data;
	err_code = _mali_ukk_attach_ump_mem( &uk_args );

    if (0 != put_user(uk_args.cookie, &argument->cookie))
    {
        if (_MALI_OSK_ERR_OK == err_code)
        {
            /* Rollback */
           	_mali_uk_release_ump_mem_s uk_args_unmap;

            uk_args_unmap.ctx = session_data;
            uk_args_unmap.cookie = uk_args.cookie;
            err_code = _mali_ukk_release_ump_mem( &uk_args_unmap );
            if (_MALI_OSK_ERR_OK != err_code)
            {
                MALI_DEBUG_PRINT(4, ("reverting _mali_ukk_attach_mem, as a result of failing put_user(), failed\n"));
            }
        }
        return -EFAULT;
    }

    /* Return the error that _mali_ukk_map_external_ump_mem produced */
	return map_errcode(err_code);
}
#endif /* MALI_USE_UNIFIED_MEMORY_PROVIDER */

int mem_query_mmu_page_table_dump_size_wrapper(struct mali_session_data *session_data, _mali_uk_query_mmu_page_table_dump_size_s __user * uargs)
{
    _mali_uk_query_mmu_page_table_dump_size_s kargs;
    _mali_osk_errcode_t err;

    MALI_CHECK_NON_NULL(uargs, -EINVAL);
    MALI_CHECK_NON_NULL(session_data, -EINVAL);

    kargs.ctx = session_data;

    err = _mali_ukk_query_mmu_page_table_dump_size(&kargs);
    if (_MALI_OSK_ERR_OK != err) return map_errcode(err);

    if (0 != put_user(kargs.size, &uargs->size)) return -EFAULT;

    return 0;
}

int mem_dump_mmu_page_table_wrapper(struct mali_session_data *session_data, _mali_uk_dump_mmu_page_table_s __user * uargs)
{
    _mali_uk_dump_mmu_page_table_s kargs;
    _mali_osk_errcode_t err;
    void *buffer;
    int rc = -EFAULT;

	/* validate input */
    MALI_CHECK_NON_NULL(uargs, -EINVAL);
	/* the session_data pointer was validated by caller */

    kargs.buffer = NULL;

    /* get location of user buffer */
	if (0 != get_user(buffer, &uargs->buffer)) goto err_exit;
	/* get size of mmu page table info buffer from user space */
	if ( 0 != get_user(kargs.size, &uargs->size) ) goto err_exit;
    /* verify we can access the whole of the user buffer */
    if (!access_ok(VERIFY_WRITE, buffer, kargs.size)) goto err_exit;

    /* allocate temporary buffer (kernel side) to store mmu page table info */
    kargs.buffer = _mali_osk_malloc(kargs.size);
    if (NULL == kargs.buffer)
    {
        rc = -ENOMEM;
        goto err_exit;
    }

    kargs.ctx = session_data;
    err = _mali_ukk_dump_mmu_page_table(&kargs);
    if (_MALI_OSK_ERR_OK != err)
    {
        rc = map_errcode(err);
        goto err_exit;
    }

    /* copy mmu page table info back to user space and update pointers */
	if (0 != copy_to_user(uargs->buffer, kargs.buffer, kargs.size) ) goto err_exit;
    if (0 != put_user((kargs.register_writes - (u32 *)kargs.buffer) + (u32 *)uargs->buffer, &uargs->register_writes)) goto err_exit;
    if (0 != put_user((kargs.page_table_dump - (u32 *)kargs.buffer) + (u32 *)uargs->buffer, &uargs->page_table_dump)) goto err_exit;
    if (0 != put_user(kargs.register_writes_size, &uargs->register_writes_size)) goto err_exit;
    if (0 != put_user(kargs.page_table_dump_size, &uargs->page_table_dump_size)) goto err_exit;
    rc = 0;

err_exit:
    if (kargs.buffer) _mali_osk_free(kargs.buffer);
    return rc;
}



int mem_get_big_block_wrapper( struct file * filp, _mali_uk_get_big_block_s __user * argument )
{
	_mali_uk_get_big_block_s uk_args;
	_mali_osk_errcode_t err_code;

	/* validate input */
	/* the session_data pointer was validated by caller */
    MALI_CHECK_NON_NULL( argument, -EINVAL);

	/* get call arguments from user space. copy_from_user returns how many bytes which where NOT copied */
	if ( 0 != copy_from_user(&uk_args, (void __user *)argument, sizeof(_mali_uk_get_big_block_s)) )
	{
		return -EFAULT;
	}

	/* This interface inserts something into the ukk_private word */
    uk_args.ukk_private = (u32)filp;
    uk_args.ctx = filp->private_data;
	err_code = _mali_ukk_get_big_block( &uk_args );

	/* Do not leak the private word back into user space */
	uk_args.ukk_private = 0;

	if ( _MALI_OSK_ERR_OK != err_code )
	{
		return map_errcode(err_code);
	}

	/* From this point on, we must roll-back any failing action to preserve the
	 * meaning of the U/K interface (e.g. when excluded) */

	/* transfer response back to user space */
	if ( 0 != copy_to_user(argument, &uk_args, sizeof(_mali_uk_get_big_block_s)) )
	{
		/* Roll-back - the _mali_uk_get_big_block call succeeded, so all
		 * values in uk_args will be correct */
		_mali_uk_free_big_block_s uk_args_rollback = {0, };

		uk_args_rollback.ctx = uk_args.ctx;
		uk_args_rollback.cookie = uk_args.cookie;
		err_code = _mali_ukk_free_big_block( &uk_args_rollback );

		if ( _MALI_OSK_ERR_OK != err_code )
		{
			/* error in DEBUG and RELEASE */
			MALI_PRINT_ERROR( ("Failed to rollback get_big_block: %.8X\n", (u32)err_code) );
		}
        return -EFAULT;
	}

    return 0;
}

int mem_free_big_block_wrapper(struct mali_session_data *session_data, _mali_uk_free_big_block_s __user * argument)
{
	_mali_uk_free_big_block_s uk_args;
	_mali_osk_errcode_t err_code;

	/* validate input */
	/* the session_data pointer was validated by caller */
    MALI_CHECK_NON_NULL( argument, -EINVAL );

	/* get call arguments from user space. get_user returns 0 on success */
	if ( 0 != get_user(uk_args.cookie, &argument->cookie) )
	{
		return -EFAULT;
	}

    uk_args.ctx = session_data;
	err_code = _mali_ukk_free_big_block( &uk_args );

	/* Return the error that _mali_ukk_free_big_block produced */
	return map_errcode(err_code);
}
