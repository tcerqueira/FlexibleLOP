-- ==== Document for the sql tables ====

-- Transform Order
CREATE TABLE IF NOT EXISTS TransformOrder (
    id_number INTEGER,
    piece_original INTEGER,
    piece_final INTEGER,
    total_amount INTEGER,
    done_amount INTEGER,
    doing_amount INTEGER,
    sent_at INTEGER,
    received_at INTEGER,
    max_delay INTEGER,
    penalty_per_day INTEGER,
    started_at INTEGER,
    finished_at INTEGER,
    penalty INTEGER,
    PRIMARY KEY (id_number),
    CHECK (penalty_per_day>=0), CHECK (penalty_per_day<=1000)
);

-- Unload Order
CREATE TABLE IF NOT EXISTS UnloadOrder (

);

-- Storage Order
CREATE TABLE IF NOT EXISTS PieceStorage (
    id_number INTEGER,
    amount INTEGER,
    PRIMARY KEY (id_number),
    CHECK (id_number>=1), CHECK (id_number<=9),
    CHECK (amount>=0)
);

-- Machine stats
CREATE TABLE IF NOT EXISTS MachineStats (
    id INTEGER,
    piece_count, 
)

-- Machined piece
CREATE TABLE IF NOT EXISTS MachinePiece (
    id INTEGER,
    p_count INTEGER
)