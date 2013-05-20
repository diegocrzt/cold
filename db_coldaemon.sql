DROP TABLE IF EXISTS pagadas, pendientes, transacciones;

CREATE TABLE pagadas(cod_serv INTEGER, tipo INTEGER, compr BIGINT, monto BIGINT, dig_verif INTEGER, prefijo INTEGER, numero INTEGER, medidor BIGINT, abonado BIGINT, transaccion INTEGER, fecha_hora BIGINT,  usuario TEXT, vencimiento BIGINT);

CREATE TABLE pendientes(cod_serv INTEGER,tipo INTEGER,compr BIGINT,monto BIGINT,dig_verif INTEGER,prefijo INTEGER,numero INTEGER ,medidor BIGINT, abonado BIGINT, id INTEGER,vencimiento BIGINT);

CREATE TABLE transacciones(operacion TEXT, cod_serv INTEGER, fecha_hora BIGINT, usuario TEXT, mensaje TEXT,tipo INTEGER, compr BIGINT, monto BIGINT, dig_verif INTEGER, prefijo INTEGER, numero INTEGER, medidor BIGINT, abonado BIGINT, transaccion INTEGER);


INSERT INTO pendientes VALUES (001,001,12345678912,000000100000,7,0,0,0,0,1,20131212235959);

INSERT INTO pendientes VALUES (002,001,0,000000100000,0,0644,9876543,0,0,2,20131212235959);

INSERT INTO pendientes VALUES (003,0,0,000000100000,0,0,0,987654321012345,0,3,20131212235959);

INSERT INTO pendientes VALUES (004,0,0,000000100000,0,0644,987654,0,0,4,20131212235959);

INSERT INTO pendientes VALUES (005,0,0,000000100000,0,0,0,0,987654321,5,20131212235959);
