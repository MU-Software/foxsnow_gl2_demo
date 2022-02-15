#include "fs_node.h"

node* create_node(node* parent_to, void(*func_in), void(*func_out), void* data, char* type) {
	node* new_node = (node*)calloc(1, sizeof(node));
	if (new_node == NULL) {
		printf("CALLOC FAILED on create_node!\n");
		exit(1);
	}

	new_node->in = func_in;
	new_node->out = func_out;

	new_node->data = data;
	new_node->type = type;

	if (parent_to != NULL) {
		if (parent_to->child != NULL) {
			node* target_neighbor = parent_to->child;
			while (target_neighbor->next != NULL)
				target_neighbor = target_neighbor->next;
			target_neighbor->next = new_node;
			new_node->prev = target_neighbor;
			new_node->parent = parent_to;
			parent_to->child_last = new_node;
		}
		else
			parent_to->child = new_node,
			parent_to->child_last = new_node,
			new_node->parent = parent_to;
	}
	return new_node;
}

int free_node(node** target_node_ptr) { //DONE
	node* target_node = *target_node_ptr;
	// Return error when target_node == NULL
	if (target_node == NULL) return 1;
	// �ڽ� ��带 ���� ������
	if (target_node->child != NULL)
		while (target_node->child)
			if (free_node(&(target_node->child))) return 1;
	// �ֺ��� ���� ������ ��尡 �ִ��� Ȯ���ϰ�, ���̸� �� ������ ���� ��带 �� ��忡 �̾��ش�
	if (target_node->next) {
		if (target_node->prev) {
			// �̷��� �갡 �׳� ���� ������ �߰��� �ִٴ°Ŵϱ� �� �յڷ� �� �̾��ָ� ��
			target_node->prev->next = target_node->next;
			target_node->next->prev = target_node->prev;
		}
		else {
			// �� ���̽��� �갡 ���� ������ ù��°���ܰŴϱ� �θ� child�� �� ���� ��带 ����ϰ�,
			// �� ���� ����� prev�� NULL�� ��� ��
			target_node->parent->child = target_node->next;
			target_node->next->prev = NULL;
		}
	}
	else {
		if (target_node->prev) {
			// ���� �갡 �� Ʈ������ ������ �������̾��ٸ� �θ��� child_last�� �ٲ��ش�
			target_node->parent->child_last = target_node->prev;
			target_node->prev->next = NULL;
		}
		else {
			// �̰� �׳� �갡 �ܵ��̾��ܰŴϱ� �θ��� child�� child_last�� ��������
			target_node->parent->child = NULL;
			target_node->parent->child_last = NULL;
		}
	}

	// ���������� �����͸� �����, �� ��� ��ü�� ������
	free(target_node->data);
	free(target_node);
	*target_node_ptr = NULL;
	return 0;
}

void node_print(node* target, int indent) {
	if (!target) {
		printf("EMPTY NODE!");
		return;
	}
	int z = 0;
	for (z; z < indent; z++) printf(" ");
	printf("NODE %s\n", target->type);

	node* target_child = target->child;
	while (target_child != NULL) {
		node_print(target_child, indent + 1);
		target_child = target_child->next;
	}
}
