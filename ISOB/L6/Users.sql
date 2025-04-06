CREATE TABLE users (
    id INT IDENTITY(1,1) PRIMARY KEY,
    username NVARCHAR(50) NOT NULL UNIQUE,
    password NVARCHAR(50) NOT NULL
);


INSERT INTO users (username, password) VALUES
('user1', 'pass1'),
('user2', 'pass2'),
('admin', 'admin');


SELECT * FROM users;

DELETE FROM users ;