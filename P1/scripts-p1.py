import sympy as sp
import math


def dot_product(v1, v2):
    return sum(a * b for a, b in zip(v1, v2))


def cross_product(v1, v2):
    return [
        v1[1] * v2[2] - v1[2] * v2[1],
        v1[2] * v2[0] - v1[0] * v2[2],
        v1[0] * v2[1] - v1[1] * v2[0]
    ]


def norm(v):
    return math.sqrt(sum(a * a for a in v))


def normalize(v):
    n = norm(v)
    return [a / n for a in v]


def matrix_vector_multiply(matrix, vector):
    return [sum(row[i] * vector[i] for i in range(len(vector))) for row in matrix]


def p_position_no_numpy():
    p_world = [3, -2, 2]  # ponto p
    c_camera = [13, 2, -7]  # posição da câmera

    d = [-c_camera[0], -c_camera[1], -c_camera[2]]  # direção é da câmera para a origem (0,0,0)

    forward = normalize(d)

    up = [0, 1, 0]

    right = normalize(cross_product(up, forward))
    true_up = cross_product(forward, right)
    camera_transform = [right, true_up, forward]

    p_camera = matrix_vector_multiply(camera_transform, [
        p_world[i] - c_camera[i] for i in range(3)
    ])

    fov_y = math.radians(60)  # FOV vertical em radianos
    aspect_ratio = 800 / 600  # razão de aspecto da imagem
    near = 1.0  # plano near
    far = 100.0  # plano far

    f = 1 / math.tan(fov_y / 2)  # fator de escala baseado no FOV

    perspective_proj = [
        [f / aspect_ratio, 0, 0, 0],
        [0, f, 0, 0],
        [0, 0, (far + near) / (near - far), (2 * far * near) / (near - far)],
        [0, 0, -1, 0]
    ]

    p_camera_homogeneous = p_camera + [1]
    p_projected_homogeneous = matrix_vector_multiply(perspective_proj, p_camera_homogeneous)

    p_projected_ndc = [p_projected_homogeneous[i] / p_projected_homogeneous[3] for i in range(3)]

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
    a = [-1, -5, -3]
    b = [7, 0, -1]
    c = [-6, 9, 0]

    # Subtração dos vetores
    ab = [a[i] - b[i] for i in range(len(a))]
    ac = [a[i] - c[i] for i in range(len(a))]

    normal_vector = cross_product(ab, ac)

    print("The normal vector of the triangle is:", normal_vector)
    print("Multiplied by 2: (the answer to the question)", 2 * normal_vector)


def main():
    #triangle_normal_vector()
    #solve_linear_system_1()  # A = O + Tx + Ly
    #solve_linear_system_2() # A = C + Tu + Lv
    p_position_no_numpy()


if __name__ == "__main__":
    main()
