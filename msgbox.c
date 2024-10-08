#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

typedef struct _msg_t msg_t;

struct _msg_t{
    msg_t* previous;
    int length;
    char* message;
};

static msg_t *bottom = NULL;
static msg_t *top = NULL;
unsigned long flags;

int msgbox_put( char *buffer, int length ) {
    local_irq_save(flags);
    if(length < 0){
        printk("length < 0 in msgbox_put\n");
        return -EINVAL; // EINVAL (alternative way to return codes) 
    } 
    if(!access_ok(buffer, length)){
        printk("access denied in msgbox_put\n");
        return -14; // EFAULT
    }
        
    msg_t* msg = kmalloc(sizeof(msg_t),GFP_KERNEL);
    msg->previous = NULL;
    msg->length = length;
    msg->message = kmalloc(length,GFP_KERNEL);
    if(!msg->message){
        printk("Couldn't allocate memory\n");
        return -12; //ENONEM
    }
    if(buffer==NULL){
        printk("Allocation failed!\n");
        return -1;
    }
    copy_from_user(msg->message, buffer, length); 
    if (bottom == NULL) {
        bottom = msg;
        top = msg;
    } else {
     /* not empty stack */
        msg->previous = top;
        top = msg;
    }
    local_irq_restore(flags);
    return 0;
}

int msgbox_get( char* buffer, int length ) {
    local_irq_save(flags);
    if(length < 0){
        printk("length < 0 in msgbox_get\n");
        return -22;
    }
    if(!access_ok(buffer, length)){
        printk("access denied in msgbox_get\n");
        return -14; 
    } 
    if (top != NULL) {
        msg_t* msg = top;
        int mlength = msg->length;
        top = msg->previous;

        /* copy message */
        copy_to_user(buffer, msg->message,length); 

        /* free memory */
        kfree(msg->message);
        kfree(msg);

        return mlength;
    }
    local_irq_restore(flags);
    return -1;


}

