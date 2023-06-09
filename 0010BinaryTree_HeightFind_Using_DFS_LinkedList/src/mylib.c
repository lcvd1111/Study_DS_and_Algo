#include "mylib.h"

//Function Definitions
BTREE_NODE *MakeChild
(BTREE_NODE *parent, CHILD_SELECTOR selector, int left, int right)
{
	if (parent==NULL){
		PRINTF("ERROR: parent is null\n");
		return NULL;
	}

	switch (selector) {
	case LEFT:
		if (parent->left != NULL){
			PRINTF("ERROR: Parent already has children(or a child).\n");
			return NULL;
		}

		parent->left = (BTREE_NODE *)malloc(sizeof(BTREE_NODE));
		
		if (parent->left == NULL){
			PRINTF("ERROR: malloc( ) failed.\n");
			return NULL;
		}
		parent->left->data = left;
		parent->left->left = NULL;
		parent->left->right = NULL;
		parent->left->header = NULL;
		break;
	case RIGHT:
		if (parent->right != NULL){
			PRINTF("ERROR: Parent already has children(or a child).\n");
			return NULL;
		}

		parent->right = (BTREE_NODE *)malloc(sizeof(BTREE_NODE));
		
		if (parent->right == NULL){
			PRINTF("ERROR: malloc( ) failed.\n");
			return NULL;
		}
		parent->right->data = right;
		parent->right->left = NULL;
		parent->right->right = NULL;
		parent->right->header = NULL;
		break;
	case BOTH:
		if ((parent->left != NULL)
			|| (parent->right != NULL)){
			PRINTF("ERROR: Parent already has children(or a child).\n");
			return NULL;
		}

		parent->left = (BTREE_NODE *)malloc(sizeof(BTREE_NODE));
		
		if (parent->left == NULL){
			PRINTF("ERROR: malloc( ) failed.\n");
			return NULL;
		}
		parent->left->data = left;
		parent->left->left = NULL;
		parent->left->right = NULL;
		parent->left->header = NULL;

		parent->right = (BTREE_NODE *)malloc(sizeof(BTREE_NODE));
		
		if (parent->right == NULL){
			PRINTF("ERROR: malloc( ) failed.\n");
			return NULL;
		}
		parent->right->data = right;
		parent->right->left = NULL;
		parent->right->right = NULL;
		parent->right->header = NULL;
		break;
	case NONE:
		break;
	default:
		PRINTF("ERROR: Wrong Selector Value.\n");
		return NULL;
		break;
	}	

	return parent;
}

STACK *CreateStack(void){

	STACK *ret = NULL;

	ret = (STACK *)malloc(sizeof(STACK));
	ret->begin = NULL;
	ret->end = NULL;

	return ret;
}

STACK *Push(STACK *stackArg, BTREE_NODE *btreeArg, TRIED_ACTION triedAction)
{
	STACK_NODE *temp = NULL;

	if (stackArg==NULL || btreeArg==NULL){
		PRINTF("ERROR: stack or btree node is NULL.\n");
		return NULL;
	}

	temp = (STACK_NODE *)malloc(sizeof(STACK_NODE));
	
	if (!temp){
		PRINTF("ERROR: malloc( ) failed.\n");
		return NULL;
	}

	temp->nodeAddress = btreeArg;
	temp->lastTried = triedAction;

	if (stackArg->begin == NULL){
		//The case when the stack is empty.
		stackArg->begin = temp;
		stackArg->end = temp;
		stackArg->begin->next = NULL;
		stackArg->begin->prev = NULL;
		return stackArg;
	}

	stackArg->end->next = temp;
	stackArg->end->next->prev = stackArg->end;
	stackArg->end->next->next = NULL;
	stackArg->end = stackArg->end->next;

	return stackArg;
}

WRAPPED_NODE *Pop(STACK *stackArg)
{
	WRAPPED_NODE *ret = NULL;
	
	if (stackArg->begin == NULL){
		//The case when the stack is empty.
		return NULL;
	}

	ret = (WRAPPED_NODE *)malloc(sizeof(WRAPPED_NODE));
	
	if (stackArg->begin == stackArg->end){
		//The case when  the stack has an signle element.
		ret->nodeAddress_W = stackArg->begin->nodeAddress;
		ret->lastTried_W = stackArg->begin->lastTried;
		free(stackArg->begin);
		stackArg->begin = NULL;
		stackArg->end = NULL;
		return ret;
	}

	ret->nodeAddress_W = stackArg->end->nodeAddress;
	ret->lastTried_W = stackArg->end->lastTried;
	stackArg->end = stackArg->end->prev;
	free(stackArg->end->next);
	stackArg->end->next = NULL;

	return ret;
}

STACK *EmptyStack(STACK *stackArg)
{
	while(stackArg->begin != NULL)
		free(Pop(stackArg));

	if (stackArg->begin != NULL)
		return NULL;

	if (stackArg->end != NULL)
		return NULL;

	return stackArg;
}

int RemoveStack(STACK *stackArg)
{
	if (stackArg->begin != NULL){
		PRINTF("ERROR: stack is not empty yet.\n");
		return -1;
	}

	free(stackArg);

	return 0;
}

int Calc_Subtree_Height(BTREE_NODE *parent, CHILD_SELECTOR direction)
{
	BTREE_NODE *temp = NULL;
	int ret = 0;
	int r_temp = 0;
	int l_temp = 0;
	switch(direction){
	case LEFT:
		temp = parent->left;
		break;
	case RIGHT:
		temp = parent->right;
		break;
	default:
		PRINTF("ERROR: Wrong direction Value.\n");
		return -1;
		break;
	}

	if (temp==NULL){
		PRINTF("ERROR: parent has no child for that direction at all.\n");
		return -2;
	}

	if (temp->header == NULL){
		PRINTF("ERROR: parent's child has no sub_tree_Height info.\n");
		return -3;
	}

	r_temp = ((HEADER_DATA*)(temp->header))->right_Sub_Height; 
	l_temp = ((HEADER_DATA*)(temp->header))->left_Sub_Height;

	ret = (l_temp>r_temp) ? l_temp : r_temp;

	ret++;


	return ret;
}

int Calc_Btree_Height_DFS(BTREE_NODE *root)
{
	BTREE_NODE *current = NULL;
	WRAPPED_NODE *wrapped_data = NULL;
	STACK *myStack = NULL;
	int branchCase = 0;
	int loop = 1;
	int ret = 0;

	current = root;
	if (root == NULL){
		return 0;
	}

	myStack = CreateStack();

	while(loop){
	switch (branchCase){
	case 0:
		//When the node is visited for the first.
		Push(myStack, current, TRIED_LEFT);
		current = current->left;

		if (current == NULL){
			wrapped_data = Pop(myStack);
			current = wrapped_data->nodeAddress_W;
			free(wrapped_data);
			current->header = (HEADER_DATA *)malloc(sizeof(HEADER_DATA));
			((HEADER_DATA *)(current->header))->left_Sub_Height = 0;
			Push(myStack, current, TRIED_RIGHT);
			current = current->right;
			if (current == NULL){
				wrapped_data = Pop(myStack);
				current = wrapped_data->nodeAddress_W;
				free(wrapped_data);
				((HEADER_DATA *)(current->header))->right_Sub_Height = 0;
				wrapped_data = Pop(myStack);
				if (wrapped_data == NULL){
					free(wrapped_data);
					loop = 0;
					ret = ((HEADER_DATA *)(current->header))->left_Sub_Height
					> ((HEADER_DATA *)(current->header))->right_Sub_Height 
					? ((HEADER_DATA *)(current->header))->left_Sub_Height 
					: ((HEADER_DATA *)(current->header))->right_Sub_Height;
					ret++;
					break;
				}
				current = wrapped_data->nodeAddress_W;
				switch(wrapped_data->lastTried_W){
				case TRIED_LEFT:
					branchCase = 1;
					break;
				case TRIED_RIGHT:
					branchCase = 2;
					break;
				default:
					PRINTF("ERROR.\n");
					return -1;
				}
			}
		}
		break;
	case 1:
		//When the last tried action was a 'Moving Left'
		current->header = (HEADER_DATA *)malloc(sizeof(HEADER_DATA));
		((HEADER_DATA *)(current->header))->left_Sub_Height = Calc_Subtree_Height(current, LEFT);
		
		Push(myStack, current, TRIED_RIGHT);
		current = current->right;
		branchCase = 0;
		if (current == NULL){
			wrapped_data = Pop(myStack);
			current = wrapped_data->nodeAddress_W;
			free(wrapped_data);
			((HEADER_DATA *)(current->header))->right_Sub_Height = 0;
			wrapped_data = Pop(myStack);
			if (wrapped_data == NULL){
				free(wrapped_data);
				loop = 0;
				ret = ((HEADER_DATA *)(current->header))->left_Sub_Height
					> ((HEADER_DATA *)(current->header))->right_Sub_Height 
					? ((HEADER_DATA *)(current->header))->left_Sub_Height 
					: ((HEADER_DATA *)(current->header))->right_Sub_Height;
				ret++;
				break;
			}
			current = wrapped_data->nodeAddress_W;
			switch(wrapped_data->lastTried_W){
			case TRIED_LEFT:
				branchCase = 1;
				break;
			case TRIED_RIGHT:
				branchCase = 2;
				break;
			default:
				PRINTF("ERROR.\n");
				return -1;
			}
		}
		break;
	case 2:
		//When the last tried action was a 'Moving Right'
		((HEADER_DATA *)(current->header))->right_Sub_Height = Calc_Subtree_Height(current, RIGHT);
		
		wrapped_data = Pop(myStack);
		if (wrapped_data == NULL){
		free(wrapped_data);
			loop = 0;
			ret = ((HEADER_DATA *)(current->header))->left_Sub_Height
					> ((HEADER_DATA *)(current->header))->right_Sub_Height 
					? ((HEADER_DATA *)(current->header))->left_Sub_Height 
					: ((HEADER_DATA *)(current->header))->right_Sub_Height;
			ret++;
			break;
		}
		current = wrapped_data->nodeAddress_W;
		switch(wrapped_data->lastTried_W){
		case TRIED_LEFT:
			branchCase = 1;
			break;
		case TRIED_RIGHT:
			branchCase = 2;
			break;
		default:
			PRINTF("ERROR.\n");
			return -1;
		}
		break;
	default:
		break;
	}
	}

	if(EmptyStack(myStack) == NULL){
		PRINTF("ERROR: EmptyStack( ) failed.\n");
		return -2;
	}

	if(RemoveStack(myStack)){
		PRINTF("ERROR: RemoveStack( ) failed.\n");
		return -3;
	}

	return ret;
}
