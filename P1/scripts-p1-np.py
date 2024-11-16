import numpy as np
import sympy as sp


def p_position_numpy():
    # Definindo os vetores posição do ponto e da câmera
    p_world = np.array([3, -2, 2])  # ponto p
    c_camera = np.array([13, 2, -7])  # posição da câmera

    # Calculando o vetor direção da câmera (apontando para a origem)
    d = -c_camera  # direção é da câmera para a origem (0,0,0)

    # Normalizando o vetor direção da câmera
    forward = d / np.linalg.norm(d)

    # Vetor "up" fornecido
    up = np.array([0, 1, 0])

    # Calculando o vetor "right" como o produto vetorial de up e forward
    right = np.cross(up, forward)
    right /= np.linalg.norm(right)

    # Recalculando "up" para garantir ortogonalidade
    true_up = np.cross(forward, right)

    # Matriz de transformação da câmera (base ortonormal)
    camera_transform = np.array([right, true_up, forward])

    # Transformando o ponto para o espaço da câmera
    p_camera = camera_transform @ (p_world - c_camera)

    # Definindo parâmetros da projeção em perspectiva
    fov_y = np.radians(60)  # FOV vertical em radianos
    aspect_ratio = 800 / 600  # razão de aspecto da imagem
    near = 1.0  # plano near (não especificado, apenas para escala)
    far = 100.0  # plano far (idem)

    # Calculando a projeção em perspectiva do ponto
    f = 1 / np.tan(fov_y / 2)  # fator de escala baseado no FOV

    # Matriz de projeção em perspectiva
    perspective_proj = np.array([
        [f / aspect_ratio, 0, 0, 0],
        [0, f, 0, 0],
        [0, 0, (far + near) / (near - far), (2 * far * near) / (near - far)],
        [0, 0, -1, 0]
    ])

    # Convertendo o ponto para coordenadas homogêneas
    p_camera_homogeneous = np.append(p_camera, 1)
    p_projected_homogeneous = perspective_proj @ p_camera_homogeneous

    # Convertendo para coordenadas de tela (dividindo por w)
    p_projected_ndc = p_projected_homogeneous[:3] / p_projected_homogeneous[3]

    # Transformando de NDC (-1,1) para coordenadas de pixel (0,800 x 0,600)
    width, height = 800, 600
    pixel_x = (p_projected_ndc[0] + 1) * width / 2
    pixel_y = (1 - p_projected_ndc[1]) * height / 2  # y é invertido

    print(f'Ponto projetado: ({pixel_x:.2f}, {pixel_y:.2f})')


def solve_linear_system_2():
    T, L = sp.symbols('T L')
    x, y = sp.symbols('x y')

    # Define the vectors as expressions
    u = 2 * x + 9 * y
    v = 6 * x + 6 * y
    c = -7 * x - 6 * y
    a = 6 * x + 5 * y

    # Set up the equation a = c + T*u + L*v
    equation = sp.Eq(a, c + T * u + L * v)

    # Separate the equation into coefficients for x and y
    x_coeff = sp.collect(sp.expand(equation.lhs - equation.rhs), x)
    y_coeff = sp.collect(sp.expand(equation.lhs - equation.rhs), y)

    # Create the system of equations
    eq1 = sp.Eq(x_coeff.coeff(x), 0)  # Coefficient of x
    eq2 = sp.Eq(y_coeff.coeff(y), 0)  # Coefficient of y

    # Solve the system of equations
    solution = sp.solve((eq1, eq2), (T, L))

    # Convert the solutions to float
    T_float = solution[T].evalf()
    L_float = solution[L].evalf()

    # Print the solution in float form
    print(f'T = {T_float:.3f}, L = {L_float:.3f}')


def solve_linear_system_1():
    x, y = sp.symbols('x y')

    u = 5 * x + y
    v = 2 * x + 2 * y
    c = 6 * x + 4 * y
    a = c + 7 * u - v

    T, L = sp.symbols('T L')

    equation = sp.Eq(a, T * x + L * y)

    expanded_equation = sp.expand(equation)

    x_coeff = expanded_equation.lhs.coeff(x) - expanded_equation.rhs.coeff(x)
    y_coeff = expanded_equation.lhs.coeff(y) - expanded_equation.rhs.coeff(y)

    eq1 = sp.Eq(x_coeff, 0)
    eq2 = sp.Eq(y_coeff, 0)

    solution = sp.solve((eq1, eq2), (T, L))

    T_float = solution[T].evalf()
    L_float = solution[L].evalf()

    print(f'T = {T_float:.3f}, L = {L_float:.3f}')


def triangle_normal_vector():
    a = np.array([-1, -5, -3])
    b = np.array([7, 0, -1])
    c = np.array([-6, 9, 0])

    ab = a - b
    ac = a - c

    normal_vector = np.cross(ab, ac)

    print("The normal vector of the triangle is:", normal_vector)
    print("Multiplied by 2: (the answer to the question)", 2 * normal_vector)


def main():
    triangle_normal_vector()
    #solve_linear_system_1() # A = O + Tx + Ly
    #solve_linear_system_2() # A = C + Tu + Lv
    #p_position_numpy()


if __name__ == "__main__":
    main()
