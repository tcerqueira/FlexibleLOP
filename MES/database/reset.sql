DELETE FROM TransformOrder;
DELETE FROM UnloadOrder;
UPDATE Machine SET total_time = 0;
UPDATE MachineStat SET piece_count = 0;
UPDATE PieceStorage SET amount = 400 WHERE piece_type = 1;
UPDATE PieceStorage SET amount = 40 WHERE piece_type = 2;
UPDATE PieceStorage SET amount = 20 WHERE piece_type = 3;
UPDATE PieceStorage SET amount = 20 WHERE piece_type = 4;
UPDATE PieceStorage SET amount = 20 WHERE piece_type = 5;
UPDATE PieceStorage SET amount = 20 WHERE piece_type = 6;