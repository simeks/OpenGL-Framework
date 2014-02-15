#include <framework/Common.h>

#include "MatrixStack.h"

#include <framework/RenderDevice.h>


MatrixStack::MatrixStack()
{
	// Push our initial state, the stack must always have at least one state.
	_states.push(State());
	_states.top().model_matrix = matrix::CreateIdentity();
	_states.top().view_matrix = matrix::CreateIdentity();
	_states.top().projection_matrix = matrix::CreateIdentity();

	_state_dirty = true;
}
MatrixStack::~MatrixStack()
{
}

void MatrixStack::Push()
{
	// Push a new copy of the current state to the top of the stack.
	_states.push(_states.top());
}
void MatrixStack::Pop()
{
	assert(_states.size() > 1); // Stack always need at least one state.
	_states.pop();
}

void MatrixStack::SetViewMatrix(const Mat4x4& view_matrix)
{
	_states.top().view_matrix = view_matrix;
	_state_dirty = true;
}
void MatrixStack::SetProjectionMatrix(const Mat4x4& projection_matrix)
{
	_states.top().projection_matrix = projection_matrix;
	_state_dirty = true;
}

void MatrixStack::Translate3f(const Vec3& translation)
{
	Mat4x4 translation_matrix = matrix::CreateTranslation(translation);

	State& current_state = _states.top();
	current_state.model_matrix = matrix::Multiply(current_state.model_matrix, translation_matrix);
	_state_dirty = true;
}
void MatrixStack::Rotate3f(float head, float pitch, float roll)
{
	Mat4x4 rotation_matrix = matrix::CreateRotationXYZ(head, pitch, roll);

	State& current_state = _states.top();
	current_state.model_matrix = matrix::Multiply(current_state.model_matrix, rotation_matrix);
	_state_dirty = true;
}
void MatrixStack::Scale3f(const Vec3& scale)
{
	Mat4x4 scale_matrix = matrix::CreateScaling(scale);

	State& current_state = _states.top();
	current_state.model_matrix = matrix::Multiply(current_state.model_matrix, scale_matrix);
	_state_dirty = true;
}
void MatrixStack::Apply(RenderDevice& render_device)
{
	if(_state_dirty)
	{
		State& current_state = _states.top();
		
		render_device.SetUniformMatrix4f("view_matrix", current_state.view_matrix);

		// model_view = view * model
		Mat4x4 model_view = matrix::Multiply(current_state.view_matrix, current_state.model_matrix);
		render_device.SetUniformMatrix4f("model_view_matrix", model_view);

		// Build our model view projection matrix
		//	model_view_projection = projection * view * model
		Mat4x4 model_view_projection = matrix::Multiply(current_state.projection_matrix, model_view);

		render_device.SetUniformMatrix4f("model_view_projection_matrix", model_view_projection);


		_state_dirty = false;
	}
}

