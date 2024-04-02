#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");
	m_particleCloudShader = CompileShaders("./Shaders/ParticleClould.vs", "./Shaders/ParticleClould.fs");
	
	//Create VBOs
	CreateVertexBufferObjects();

	//Create Particle Cloud
	CreateParticleCloud(10000);

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);


	float vertices[] = { 0.0f , 0.0f, 0.0f
						,1.0f , 0.0f, 0.0f 
						,1.0f , 1.0f, 0.0f 
	};

	glGenBuffers(1, &m_TestVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_TestVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	float size = 0.05f;
	float particleVerts[] = {
		-size , -size ,0,
		size , -size , 0,
		size, size, 0,

		size , size ,0,
		-size , size , 0,
		-size, -size, 0,

	};

	glGenBuffers(1, &m_particleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleVerts), particleVerts, GL_STATIC_DRAW);

	


}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//���̴� ������Ʈ ����
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//���̴� �ڵ带 ���̴� ������Ʈ�� �Ҵ�
	glShaderSource(ShaderObj, 1, p, Lengths);

	//�Ҵ�� ���̴� �ڵ带 ������
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj �� ���������� ������ �Ǿ����� Ȯ��
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL �� shader log �����͸� ������
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram �� attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //�� ���̴� ���α׷� ����

	if (ShaderProgram == 0) { //���̴� ���α׷��� ����������� Ȯ��
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs �� vs ������ �ε���
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs �� fs ������ �ε���
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram �� vs.c_str() ���ؽ� ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram �� fs.c_str() �����׸�Ʈ ���̴��� �������� ����� attach��
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach �Ϸ�� shaderProgram �� ��ŷ��
	glLinkProgram(ShaderProgram);

	//��ũ�� �����ߴ��� Ȯ��
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program �α׸� �޾ƿ�
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done.";

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::CreateParticleCloud(int numParticles)
{

	float centerX, centerY;
	centerX = 0.0f;
	centerY = 0.0f;

	float size = 0.005f;
	int particleCount = numParticles;
	int vertexCount = particleCount * 6;
	int floatCount = vertexCount * (3 + 3 +1 +1 +1 + 1 + 1);
	//x ,y , z , vx ,vy ,vz �̷��� ����




	float* vertices = NULL;
	vertices = new float[floatCount];
	float vx, vy, vz;
	float startTime = 0;
	float lifeTime = 0;
	float amp, period;
	float value;

	int index = 0;
	for (int i = 0; i < particleCount; i++){

		float velocityScalee = 0.2f;
		/*centerX = ((float)rand() / (float)RAND_MAX)* 2.f - 1.f;
		centerY = ((float)rand() / (float)RAND_MAX) * 2.f - 1.f;*/

		centerX = 0;
		centerY = 0;
		
		//vx = (((float)rand() / (float)RAND_MAX) * 2.f - 1.f)* velocityScalee;
		vy = -0.01f + (((float)rand() / (float)RAND_MAX) * 2.f - 1.f ) * velocityScalee;
		vz = (((float)rand() / (float)RAND_MAX) * 2.f - 1.f) * velocityScalee;
		vx = 0.0f;
		//vy = 0.5f;
		//vz = 0.0f;

		startTime = 1 * ((float)rand() / (float)RAND_MAX);
		lifeTime = 1 * ((float)rand() / (float)RAND_MAX) + 1.f;

		//startTime = 0;
		//lifeTime = 2.f;

		amp = (((float)rand() / (float)RAND_MAX) - 0.5f) * 2.0f;
		period = ((float)rand() / (float)RAND_MAX);
		value = ((float)rand() / (float)RAND_MAX);

		vertices[index++] = centerX - size;
		vertices[index++] = centerY - size;
		vertices[index++] = 0.f;
		vertices[index++] = vx;
		vertices[index++] =	vy;
		vertices[index++] =	vz;
		vertices[index++] = startTime;
		vertices[index++] = lifeTime;
		vertices[index++] = amp;
		vertices[index++] = period;
		vertices[index++] = value;

		vertices[index++] = centerX + size;
		vertices[index++] = centerY + size;
		vertices[index++] = 0.f;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = startTime;
		vertices[index++] = lifeTime;
		vertices[index++] = amp;
		vertices[index++] = period;
		vertices[index++] = value;

		vertices[index++] = centerX - size;
		vertices[index++] = centerY + size;
		vertices[index++] = 0.f;   ///triangle1
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = startTime;
		vertices[index++] = lifeTime;
		vertices[index++] = amp;
		vertices[index++] = period;
		vertices[index++] = value;

		vertices[index++] = centerX - size;
		vertices[index++] = centerY - size;
		vertices[index++] = 0;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = startTime;
		vertices[index++] = lifeTime;
		vertices[index++] = amp;
		vertices[index++] = period;
		vertices[index++] = value;

		vertices[index++] = centerX + size;;
		vertices[index++] = centerY - size;
		vertices[index++] = 0.f;
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = startTime;
		vertices[index++] = lifeTime;
		vertices[index++] = amp;
		vertices[index++] = period;
		vertices[index++] = value;

		vertices[index++] = centerX + size;
		vertices[index++] = centerY + size;
		vertices[index++] = 0.f;   ///triangle1
		vertices[index++] = vx;
		vertices[index++] = vy;
		vertices[index++] = vz;
		vertices[index++] = startTime;
		vertices[index++] = lifeTime;
		vertices[index++] = amp;
		vertices[index++] = period;
		vertices[index++] = value;
	}

	glGenBuffers(1, &m_particleCloudVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, vertices, GL_STATIC_DRAW);
	m_particleCloudVertexCount = vertexCount;

	delete[] vertices;
}


void Renderer::DrawTest()
{

	//Program select
	glUseProgram(m_SolidRectShader);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_TestVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

}

void Renderer::DrawParticle()
{
	GLuint shader = m_particleShader;
	//Program select
	glUseProgram(shader);
	m_particleTime += 0.016f; //��Ȯ���� �ʴ�.
	if (m_particleTime > 200.f) m_particleTime = 0;
	int ulTime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(ulTime, m_particleTime);


	int ulPeriod = glGetUniformLocation(shader, "u_Period");
	glUniform1f(ulPeriod, 2.f);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);
}


void Renderer::DrawParticleCloud(){

	GLuint shader = m_particleCloudShader;
	//Program select
	glUseProgram(shader);
	m_particleTime += 0.016; //��Ȯ���� �ʴ�.
	if (m_particleTime > 200.f) m_particleTime = 0;
	GLuint  stride = sizeof(float) * 11;
	int ulTime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(ulTime, m_particleTime);


	int ulPeriod = glGetUniformLocation(shader, "u_Period");
	glUniform1f(ulPeriod, 1.f);


	int ulAcc = glGetUniformLocation(shader, "u_Acc");
	glUniform2f(ulAcc, cos(m_particleTime) / 10 , sin(m_particleTime)/10);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, stride, 0);

	int attribVelociy = glGetAttribLocation(shader, "a_Velocity");
	glEnableVertexAttribArray(attribVelociy);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glVertexAttribPointer(attribVelociy, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)*3));


	int attribTime = glGetAttribLocation(shader, "a_StartTime");
	glEnableVertexAttribArray(attribTime);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glVertexAttribPointer(attribTime, 1, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));

	int attribLifeTime = glGetAttribLocation(shader, "a_LifeTime");
	glEnableVertexAttribArray(attribLifeTime);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glVertexAttribPointer(attribLifeTime, 1, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 7));

	int attribAmp = glGetAttribLocation(shader, "a_Amp");
	glEnableVertexAttribArray(attribAmp);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glVertexAttribPointer(attribAmp, 1, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 8));


	int attribPeriod = glGetAttribLocation(shader, "a_Period");
	glEnableVertexAttribArray(attribPeriod);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glVertexAttribPointer(attribPeriod, 1, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 9));

	int attribValue = glGetAttribLocation(shader, "a_Value");
	glEnableVertexAttribArray(attribValue);
	glBindBuffer(GL_ARRAY_BUFFER, m_particleCloudVBO);
	glVertexAttribPointer(attribValue, 1, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 10));

	glDrawArrays(GL_TRIANGLES, 0, m_particleCloudVertexCount);

	glDisableVertexAttribArray(attribPosition);
}