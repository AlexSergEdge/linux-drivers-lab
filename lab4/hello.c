/* hello.c – Простой загружаемый модуль ядра */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/sched.h>

// Функция печатает все процессы ядра
static void print_process_info(void) {
	// Структура типа task_struct
	struct task_struct * task_list;
	size_t task_counter = 0;
	// Для каждого процесса
	for_each_process(task_list) {
		// Отображаем имя, id и текущий cpu
		printk(KERN_INFO "name: %-17s pid:%-3d\n", task_list->comm, task_list->pid);
		++task_counter;
	}
	// Печатаем общее число процессов
	printk(KERN_INFO "total number of processes: %d\n", task_counter);
}

static int __init hello_init(void) /* Инициализация */
{
	printk(KERN_INFO "Hello: registered\n");
	print_process_info();
	return 0;
}

static void __exit hello_exit(void) /* Деинициализаия */
{
	printk(KERN_INFO "Hello: unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Sidyakin");
MODULE_DESCRIPTION("Simple loadable kernel module");