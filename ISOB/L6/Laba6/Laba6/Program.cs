using System;
using System.Data.SqlClient;
using System.Windows.Forms;
using Microsoft.Data.SqlClient;

namespace SqlInjectionDemo
{
    public class Form1 : Form
    {
        private TextBox textBoxUsername;
        private TextBox textBoxPassword;
        private TextBox textBoxConfirmPassword;
        private Button buttonRegister;
        private Button buttonLogin;
        private ComboBox comboBoxMode;
        private Label labelMessage;

        private string connectionString = @"Data Source=LAVR\SQLEXPRESS;Initial Catalog=ISOB;Integrated Security=True;Connect Timeout=30;Encrypt=True;Trust Server Certificate=True;Application Intent=ReadWrite;Multi Subnet Failover=False";

        public Form1()
        {
            textBoxUsername = new TextBox { PlaceholderText = "Имя пользователя", Top = 10, Left = 10, Width = 200 };
            textBoxPassword = new TextBox { PlaceholderText = "Пароль", Top = 40, Left = 10, Width = 200, PasswordChar = '*' };
            textBoxConfirmPassword = new TextBox { PlaceholderText = "Подтверждение пароля", Top = 70, Left = 10, Width = 200, PasswordChar = '*' };
            buttonRegister = new Button { Text = "Регистрация", Top = 100, Left = 10, Width = 100 };
            buttonLogin = new Button { Text = "Вход", Top = 100, Left = 120 };
            comboBoxMode = new ComboBox { Top = 130, Left = 10, Width = 200 };
            comboBoxMode.Items.Add("Без защиты");
            comboBoxMode.Items.Add("С защитой");
            comboBoxMode.SelectedIndex = 0; // По умолчанию без защиты
            labelMessage = new Label { Top = 160, Left = 10, Width = 300 };

            buttonRegister.Click += ButtonRegister_Click;
            buttonLogin.Click += ButtonLogin_Click;

            Controls.Add(textBoxUsername);
            Controls.Add(textBoxPassword);
            Controls.Add(textBoxConfirmPassword);
            Controls.Add(buttonRegister);
            Controls.Add(buttonLogin);
            Controls.Add(comboBoxMode);
            Controls.Add(labelMessage);
        }

        private void ButtonRegister_Click(object sender, EventArgs e)
        {
            string username = textBoxUsername.Text.Trim();
            string password = textBoxPassword.Text.Trim();
            string confirmPassword = textBoxConfirmPassword.Text.Trim();

            if (string.IsNullOrEmpty(username))
            {
                labelMessage.Text = "Имя пользователя не может быть пустым.";
                return;
            }

            if (string.IsNullOrEmpty(password) || password.Length < 4)
            {
                labelMessage.Text = "Пароль должен содержать не менее 4 символов.";
                return;
            }

            if (password != confirmPassword)
            {
                labelMessage.Text = "Пароли не совпадают.";
                return;
            }

            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                connection.Open();
                string query = "INSERT INTO Users (Username, Password) VALUES (@username, @password)";
                using (SqlCommand command = new SqlCommand(query, connection))
                {
                    command.Parameters.AddWithValue("@username", username);
                    command.Parameters.AddWithValue("@password", password);
                    command.ExecuteNonQuery();
                }
            }
            labelMessage.Text = "Регистрация успешна!";
        }

        private void ButtonLogin_Click(object sender, EventArgs e)
        {
            string username = textBoxUsername.Text.Trim();
            string password = textBoxPassword.Text.Trim();

            bool isSafeMode = comboBoxMode.SelectedIndex == 1;

            if (isSafeMode)
            {
                LoginSecure(username, password);
            }
            else
            {
                LoginInsecure(username, password);
            }
        }

        private void LoginSecure(string username, string password)
        {
            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                connection.Open();
                string query = "SELECT COUNT(*) FROM Users WHERE Username = @username AND Password = @password";
                using (SqlCommand command = new SqlCommand(query, connection))
                {
                    //  для предотвращения инъекций
                    command.Parameters.AddWithValue("@username", username);
                    command.Parameters.AddWithValue("@password", password);

                    int count = (int)command.ExecuteScalar();
                    labelMessage.Text = count > 0 ? "Вход успешен!" : "Неправильное имя пользователя или пароль.";
                }
            }
        }

        private void LoginInsecure(string username, string password)
        {
            string query = $"SELECT COUNT(*) FROM Users WHERE Username = '{username}' AND Password = '{password}'";

            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                connection.Open();
                using (SqlCommand command = new SqlCommand(query, connection))
                {
                    int count = (int)command.ExecuteScalar();
                    labelMessage.Text = count > 0 ? "Вход успешен (возможно, это не безопасно)!" : "Неправильное имя пользователя или пароль.";
                }
            }
        }

        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}


//' OR '1'='1 
//  '; UPDATE Users SET Password='hacked' WHERE Username='admin' --
//'; DROP TABLE Users --
// admin' -- 